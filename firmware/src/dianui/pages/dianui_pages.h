#ifndef DIANUI_PAGES_H
#define DIANUI_PAGES_H

#include "../core/dianui_types.h"

void dianui_update_page(void);

void dianui_page_input(DianUI_NavEvent event);

void dianui_router_push(int page_id);

bool dianui_router_pop(void);

void dianui_init_router(const DianUI_Page *const *table, int table_size, int root_page_id);

#endif // DIANUI_PAGES_H