#include "dianui_pages.h"
#include "dianui_engine.h"
#include <string.h>

DianUI_Page *current_page = NULL;

void dianui_load_page(DianUI_Page *page)
{
    if (current_page && current_page->on_load)
    {
        dianui_reset();
    }

    current_page = page;

    if (current_page && current_page->on_load)
    {
        current_page->on_load();
    }

    dianui_render();
}