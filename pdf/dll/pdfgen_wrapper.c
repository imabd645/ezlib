#include "pdfgen.h"
#include <stdint.h>

__declspec(dllexport) struct pdf_doc* pdf_create_ez(int width_100, int height_100) {
    return pdf_create(width_100 / 100.0f, height_100 / 100.0f, NULL);
}

__declspec(dllexport) int pdf_add_text_ez(struct pdf_doc *pdf, struct pdf_object *page, const char *text, int size_100, int x_100, int y_100, uint32_t colour) {
    return pdf_add_text(pdf, page, text, size_100 / 100.0f, x_100 / 100.0f, y_100 / 100.0f, colour);
}

__declspec(dllexport) int pdf_add_line_ez(struct pdf_doc *pdf, struct pdf_object *page, int x1_100, int y1_100, int x2_100, int y2_100, int width_100, uint32_t colour) {
    return pdf_add_line(pdf, page, x1_100 / 100.0f, y1_100 / 100.0f, x2_100 / 100.0f, y2_100 / 100.0f, width_100 / 100.0f, colour);
}

__declspec(dllexport) int pdf_add_rectangle_ez(struct pdf_doc *pdf, struct pdf_object *page, int x_100, int y_100, int w_100, int h_100, int border_100, uint32_t colour) {
    return pdf_add_rectangle(pdf, page, x_100 / 100.0f, y_100 / 100.0f, w_100 / 100.0f, h_100 / 100.0f, border_100 / 100.0f, colour);
}

__declspec(dllexport) int pdf_add_filled_rectangle_ez(struct pdf_doc *pdf, struct pdf_object *page, int x_100, int y_100, int w_100, int h_100, int border_100, uint32_t colour_fill, uint32_t colour_border) {
    return pdf_add_filled_rectangle(pdf, page, x_100 / 100.0f, y_100 / 100.0f, w_100 / 100.0f, h_100 / 100.0f, border_100 / 100.0f, colour_fill, colour_border);
}

__declspec(dllexport) int pdf_add_circle_ez(struct pdf_doc *pdf, struct pdf_object *page, int x_100, int y_100, int radius_100, int width_100, uint32_t colour, uint32_t fill_colour) {
    return pdf_add_circle(pdf, page, x_100 / 100.0f, y_100 / 100.0f, radius_100 / 100.0f, width_100 / 100.0f, colour, fill_colour);
}

__declspec(dllexport) int pdf_add_image_file_ez(struct pdf_doc *pdf, struct pdf_object *page, int x_100, int y_100, int w_100, int h_100, const char *image_filename) {
    return pdf_add_image_file(pdf, page, x_100 / 100.0f, y_100 / 100.0f, w_100 / 100.0f, h_100 / 100.0f, image_filename);
}
