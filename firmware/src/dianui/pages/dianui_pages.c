#include "dianui_pages.h"
#include "../core/dianui_engine.h"
#include "../core/dianui_time.h"
#include <string.h>

static const DianUI_Page *const *page_table = NULL;
static int page_count = 0;

static const DianUI_Page *current_page = NULL;
static int current_page_id = -1;

#define DIANUI_MAX_PAGE_HISTORY 8

static int history[DIANUI_MAX_PAGE_HISTORY];
static int history_top = -1;

static void dianui_load_page(int page_id);

/* Router */

void dianui_init_router(const DianUI_Page *const *table, int table_size, int root_page_id)
{
    page_table = table;
    page_count = table_size;

    if (root_page_id < 0 || root_page_id >= table_size)
        return;

    current_page_id = root_page_id;
    current_page = page_table[current_page_id];

    if (current_page && current_page->on_enter)
        current_page->on_enter();
}

void dianui_router_push(int page_id)
{
    if (page_id >= page_count || page_id == current_page_id || page_id < 0)
        return;

    if (history_top < DIANUI_MAX_PAGE_HISTORY - 1)
    {
        history[++history_top] = current_page_id;
    }

    dianui_load_page(page_id);
}

bool dianui_router_pop(void)
{
    if (history_top < 0)
        return false;

    int prev_page_id = history[history_top--];
    dianui_load_page(prev_page_id);
    return true;
}

/* Page */

static void dianui_load_page(int page_id)
{
    if (!page_table || page_id < 0 || page_id >= page_count || page_id == current_page_id)
        return;

    if (current_page && current_page->on_exit)
        current_page->on_exit();

    dianui_reset();

    current_page_id = page_id;
    current_page = page_table[current_page_id];

    if (current_page && current_page->on_enter)
        current_page->on_enter();

    dianui_render();
}

void dianui_update_page(void)
{
    if (current_page && current_page->on_update)
        current_page->on_update(dianui_time_get_dt());
}

void dianui_page_input(DianUI_NavEvent event)
{
    if (current_page && current_page->on_input)
        current_page->on_input(event);
}