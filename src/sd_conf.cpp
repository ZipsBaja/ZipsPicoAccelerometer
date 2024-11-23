#include <SDCard.h>

uazips::OrderedList<spi_t*> uazips::SDCard::spis = uazips::OrderedList<spi_t*>();
uazips::OrderedList<sd_card_t*> uazips::SDCard::sd_cards = uazips::OrderedList<sd_card_t*>();

size_t sd_get_num()
{
    return uazips::SDCard::sd_cards.GetSize();
}
sd_card_t* sd_get_by_num(size_t num)
{
    return uazips::SDCard::sd_cards.Get(num);
}
size_t spi_get_num()
{
    return uazips::SDCard::spis.GetSize();
}
spi_t* spi_get_by_num(size_t num)
{
    return uazips::SDCard::spis.Get(num);
}