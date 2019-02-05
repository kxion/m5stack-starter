#pragma once

void display_init();
void display_set_header(const char *hdr);
/* Call periodicially to update the status bar information */
void display_update();