#pragma once

#include <string.h>

#include <ZipsLib.h>
#include <OrderedList.h>

#include <rtc.h>
#include <f_util.h>
#include <ff.h>
#include <hw_config.h>

namespace uazips
{
    class SDCard
    {
    private:
        spi_t* m_spi;
        sd_card_t* m_sd_card;
        FRESULT m_result;
        FIL m_file;
    public:
        static OrderedList<spi_t*> spis;
        static OrderedList<sd_card_t*> sd_cards;

    public:
        inline SDCard(const char* sys_name, spi_inst_t* spi_inst, uint8_t miso_pin, uint8_t mosi_pin, uint8_t sck_pin, uint8_t cs_pin)
        {
            m_spi = new spi_t;
            if (!m_spi)
                THROW("Out of memory.");
            memset(m_spi, 0, sizeof(spi_t));
            m_spi->hw_inst = spi_inst;
            m_spi->miso_gpio = miso_pin;
            m_spi->mosi_gpio = mosi_pin;
            m_spi->sck_gpio = sck_pin;
            m_spi->baud_rate = 12500 * 1000;

            m_sd_card = new sd_card_t;
            if (!m_sd_card)
                THROW("Out of memory.");
            memset(m_sd_card, 0, sizeof(sd_card_t));
            m_sd_card->pcName = sys_name;
            m_sd_card->spi = m_spi;
            m_sd_card->ss_gpio = cs_pin;
            m_sd_card->use_card_detect = 0;

            spis.Push(m_spi);
            sd_cards.Push(m_sd_card);
        }

        inline ~SDCard()
        {
            spis.Remove(m_spi);
            sd_cards.Remove(m_sd_card);
            delete m_spi;
            delete m_sd_card;
        }

        inline void Mount()
        {
            m_result = f_mount(&m_sd_card->fatfs, m_sd_card->pcName, 1);
            if (m_result != FR_OK)
            {
                f_unmount(m_sd_card->pcName);
                THROW("Failed to mount SD Card.");
            }
        }

        inline void OpenFile(const char* file_name)
        {
            m_result = f_open(&m_file, file_name, FA_OPEN_APPEND | FA_WRITE);
            if (m_result != FR_OK && m_result != FR_EXIST)
            {
                f_unmount(m_sd_card->pcName);
                THROW("Could not open file \"%s\".", file_name);
            }
        }

        inline void WriteString(const char* str)
        {
            f_puts(str, &m_file);
        }

        inline void WriteChar(TCHAR c)
        {
            f_putc(c, &m_file);
        }

        inline void WriteBuff(const void* buff, size_t byte_amt)
        {
            size_t bytes_written;
            f_write(&m_file, buff, byte_amt, &bytes_written);
            if (bytes_written < byte_amt)
                THROW("Disk space full!");
        }

        inline void Close()
        {
            m_result = f_close(&m_file);
            if (m_result != FR_OK)
                THROW("Could not close file.");
        }

    };
}