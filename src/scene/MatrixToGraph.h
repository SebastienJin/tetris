#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>
#include <gdkmm/color.h>

#include "../piece.h"

#define width 30
#define height 30
#define dis_w 2
#define dis_h 2


class MatrixToGraph : public Gtk::DrawingArea
{
public:

    Matrix<piece_type> &matrix;

    Gdk::Color piece_colored(piece_type t)
    {
        Gdk::Color c;
        switch (t)
        {
        case I:
            c.set_rgb_p(0.0, 1.0, 1.0);
            break;
        case O:
            c.set_rgb_p(1.0, 1.0, 0.0);
            break;
        case T:
            c.set_rgb_p(1.0, 1.0, 0.4);
            break;
        case L:
            c.set_rgb_p(1.0, 0.4, 1.0);
            break;
        case J:
            c.set_rgb_p(0.4, 0.4, 1.0);
            break;
        case Z:
            c.set_rgb_p(1.0, 0.4, 0.4);
            break;
        case S:
            c.set_rgb_p(0.4, 1.0, 0.4);
            break;
        case NONE:
            c.set_rgb_p(0.7, 0.7, 0.7);
            break;
        }
        return c;
    }
    
    MatrixToGraph(Matrix<piece_type> &matrix) : matrix(matrix)
    {
        set_margin_top(15);
        set_size_request((width + dis_w) * ((int)matrix.cols_value()),
                         (height + dis_h) * ((int)matrix.rows_value()));
    }

    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
    {
        for (int i = 0; i < matrix.rows_value(); i++)
        {
            for (int j = 0; j < matrix.cols_value(); j++)
            {
                Gdk::Color c = piece_colored(matrix.position_value(i, j));
                cr->set_source_rgba(c.get_red_p(), c.get_green_p(), c.get_blue_p(), 1.0);
                cr->rectangle((width + dis_w) * j, (height + dis_h) * i, width, height);
                cr->fill();
            }
        }
        return true;
    }

};
