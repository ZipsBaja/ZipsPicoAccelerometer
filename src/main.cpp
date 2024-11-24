#define USING_PRINT 0
#define USING_MULTIPLEXING 1
#define USING_ENCRYPTION 0
#define USING_GYRO 0

#define INTERVAL_MS 100

#include <string>

#include <TimeHandler.h>
#include <ZipsMPU6050.h>
#include <SDCard.h>
#include <SDUtil.h>

#if USING_MULTIPLEXING
#define CALIBRATION_ITERATIONS 25000
#define MPU0_PIN 2
#define MPU1_PIN 3
#define MPU2_PIN 6
#define MPU3_PIN 7
#define MPU4_PIN 8
#define MPU5_PIN 9
#include <VirtualMultiplexer.h>
#else
#define CALIBRATION_ITERATIONS 50000
#define MPU0_PIN 0
#define MPU1_PIN 0
#define MPU2_PIN 0
#define MPU3_PIN 0
#define MPU4_PIN 0
#define MPU5_PIN 0
#endif


#if USING_ENCRYPTION
#define DATA_ENTRY_SIZE 22
#define VECTOR_ENTRY_SIZE 12
#define TIME_ENTRY_SIZE 8
#define PIN_ENTRY_SIZE 1
#define SPACER_ENTRY_SIZE 1
#else
#define DATA_ENTRY_SIZE 0
#define VECTOR_ENTRY_SIZE 0
#define TIME_ENTRY_SIZE 0
#define PIN_ENTRY_SIZE 0
#define SPACER_ENTRY_SIZE 0
#endif

using namespace uazips;

int main()
{
#if USING_ENCRYPTION
	// Vector size + Time size + Pin number size + Data Spacer should not be greater than the total size each write.
	if ((VECTOR_ENTRY_SIZE + TIME_ENTRY_SIZE + PIN_ENTRY_SIZE + SPACER_ENTRY_SIZE) > DATA_ENTRY_SIZE)
	{
		THROW("Size of vector and time data larger than data entry size!");
	}
#endif

	init_libs();
	BEGIN_SETUP();

	// MPU6050
	gpio_init(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_init(PICO_DEFAULT_I2C_SCL_PIN);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

	//HW125
	gpio_init(SD_CARD_SCK_PIN);
	gpio_init(SD_CARD_TX_PIN);
	gpio_init(SD_CARD_RX_PIN);
	gpio_init(SD_CARD_CS_PIN);
	gpio_set_function(SD_CARD_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SD_CARD_TX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SD_CARD_RX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SD_CARD_CS_PIN, GPIO_FUNC_SPI);
	gpio_pull_up(SD_CARD_SCK_PIN);
	gpio_pull_up(SD_CARD_TX_PIN);
	gpio_pull_up(SD_CARD_RX_PIN);
	gpio_pull_up(SD_CARD_CS_PIN);
	
	MPU6050 mpu(0x69);
	SDCard card("0:", spi1, SD_CARD_RX_PIN, SD_CARD_TX_PIN, SD_CARD_SCK_PIN, SD_CARD_CS_PIN);
	TimeHandler th;

#if USING_ENCRYPTION
	const char* file_name = "acceleration.raw";
#else
	const char* file_name = "acceleration.txt";
#endif

#if USING_MULTIPLEXING
	uint8_t pins[] = {MPU0_PIN, MPU1_PIN, MPU2_PIN, MPU3_PIN, MPU4_PIN, MPU5_PIN};
	VirtualMultiplexer multiplex(pins, count_of(pins));
	multiplex.InitializePins();
	size_t pin_count = multiplex.GetTotalPins();
	Vec3f offsets[pin_count];
	for (int i = 0; i < pin_count; i++)
	{
		multiplex.SetHighByIndex(i);
		offsets[i] = mpu.CalibrateAcceleration(CALIBRATION_ITERATIONS);
	}
#else
	mpu.CalibrateAcceleration(CALIBRATION_ITERATIONS);
#endif

	card.Mount();

	BEGIN_LOOP();
	while (1)
	{
#if USING_MULTIPLEXING
		for (uint8_t i = 0; i < pin_count; i++) // i is the current high pin.
		{
			multiplex.SetHighByIndex(i);
#else
			uint8_t i = 0;
#endif
			card.OpenFile(file_name);

			sleep_ms(INTERVAL_MS); // Delay

			mpu.Update();
			th.Update();

#if USING_MULTIPLEXING
			Vec3f v = mpu.GetAccelerationNoOffset() - offsets[i];
#else
			Vec3f v = mpu.GetAcceleration();
#endif
			uint64_t time = th.GetElapsed();

			float accel[3] = {v.x, v.y, v.z};

			LOG("Acceleration in m/s^2: <%f, %f, %f>\n", v.x, v.y, v.z);

#if USING_ENCRYPTION
			uint8_t block[DATA_ENTRY_SIZE];

			const size_t vect_size = sizeof(accel);
			memcpy(block, accel, vect_size);
			memcpy(block + vect_size, &time, sizeof(time));
#if USING_MULTIPLEXING
			block[DATA_ENTRY_SIZE - (PIN_ENTRY_SIZE + SPACER_ENTRY_SIZE)] = multiplex.GetPinByIndex(i);
#else
			block[DATA_ENTRY_SIZE - (PIN_ENTRY_SIZE + SPACER_ENTRY_SIZE)] = 0;
#endif
			block[DATA_ENTRY_SIZE - SPACER_ENTRY_SIZE] = 0xFF; // Spacer byte used for separating data entries.

			card.WriteBuff(block, DATA_ENTRY_SIZE);
#else
			char pre[256] = "Acceleration vector at hardware time ";
			char str[256 - 64];
			sprintf(str, "%i: <%f, %f, %f> on pin number: %i\n", time, v.x, v.y, v.z, i);
			strcat(pre, str);
			card.WriteString(pre);
#endif
			card.Close();
#if USING_MULTIPLEXING
		}
#endif
	}

}