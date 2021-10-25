#ifndef _MDISP_h
#define _MDISP_h


#include <stdbool.h>
#include "point_types.h"
#include "config.h"
#include "init_seq.h"
#include "mdisp_device.h"

#ifndef MDISP_CPU
	#define MDISP_CPU 1
#endif

// main defines

#define MDISP_DATA 1                  // send data as data
#define MDISP_CMD  0                  // send data as commands

#define MDISP_FONT8x6_EN              // enable small font 8x6
#define MDISP_FONT8x6_FULL_CHARSET    // cyrilic extension (CP-1251) of font 8x6
#define MDISP_FONT8x6_VERTICAL_EN     // vertical (down to up) 8x6 charset text
//
#define MDISP_FONT16x10_EN            // enable medium font 16x10
#define MDISP_FONT16x10_FULL_CHARSET  // cyrilic extension (CP-1251) of font 16x10

#define MDISP_FONT32x24_EN            // enable large digits 32x24

#define MDISP_GRAPHIC_EN              // enable graphics functions
#define MDISP_GRAPH_TRIANGLE_EN       // enable triangle drawing

#define MDISP_BUFF_EN                 // enable buffer
#define MDISP_DEFERRED_FLUSH          // deferred update screen with "MDISP_flush" function
#define MDISP_BEZIER_MAXP     8       // maximum number of Bezier base points

#ifdef MDISP_DEFERRED_FLUSH
	#ifndef MDISP_BUFF_EN
		#error "MDISP_DEFERRED_FLUSH" must be defined with "MDISP_BUFF_EN"
	#endif
#endif


//------------------------------------------------------------------------------

#define MDISP_FILL_NEG 0x80  // negative flag of MDISP_OverwriteMode
// posible values of MDISP_OverwriteMode
enum e_MDISP_OverwriteMode
{
	MDISP_FILL_OVERWRITE     = 0,
	MDISP_FILL_OR,
	MDISP_FILL_AND,
	MDISP_FILL_XOR,
	MDISP_FILL_NEG_OVERWRITE = MDISP_FILL_NEG,
	MDISP_FILL_NEG_OR,
	MDISP_FILL_NEG_AND,
	MDISP_FILL_NEG_XOR,
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//typedef void (*fp_out_buff)(const uint8_t *data, int num, int data_cmd);
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class Mdisp
{
	public:
		// methods
		Mdisp(int16_t width, int16_t height, MdispDevice &disp, const uint8_t * init_seq, int init_seq_size);
		//Mdisp(uint8_t width, uint8_t height, const struct proc_param * Pp);
		~Mdisp();

		int16_t Width(void){return width1;}
		int16_t Height(void){return height1;}

		void OverwriteMode(enum e_MDISP_OverwriteMode m){m_OverwriteMode = m;}

		void  clear(void);
		void  flush(bool force);
		char  set_x1y8(int16_t x1, int16_t y8);
		void  send_x1y8(int16_t x1, int16_t y8);

		#ifdef MDISP_GRAPHIC_EN
			void  pixel(int16_t x, int16_t y, uint8_t val);
			void  line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val); // do not use it for vertical lines - it does not work :( Use MDISP_fill_rect vith same X
			void  rect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val, bool fill);
			void  rect_fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val);
			void  rect_holl(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t val);
			void  cyrcle(int16_t x, int16_t y, int16_t radius, uint8_t val, bool fill);
			#ifdef MDISP_GRAPH_TRIANGLE_EN
				void  triangle     (int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t val, bool fill);
				void  triangle_fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t val);
				void  triangle_holl(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t val);
				void  triangle_fill(const struct point P[],  uint8_t val);
				void  triangle_holl(const struct point P[],  uint8_t val);
				void  triangle_fill(const struct point &p1,  const struct point &p2,  const struct point &p3,  uint8_t val);
				void  triangle_holl(const struct point &p1,  const struct point &p2,  const struct point &p3,  uint8_t val);
			#endif  // MDISP_GRAPH_TRIANGLE_EN
			#ifdef MDISP_BEZIER_MAXP
				void bezier(struct point *P, short p_num, short t_num, uint8_t val);
			#endif  // MDISP_BEZIER_MAXP

		#endif  // MDISP_GRAPHIC_EN

		#ifdef MDISP_FONT8x6_EN
			void  text_8x6_putc(uint8_t c);
			void  text_8x6_puts(char *s);
			void  text_8x6_new_line(void);
			void  text_8x6_scroll(int16_t L8);

			#ifdef MDISP_FONT8x6_VERTICAL_EN
				void  text_8x6_putc_v(uint8_t c, int16_t x, int16_t y);
				void  text_8x6_puts_v(char *s, int16_t x, int16_t y);
			#endif // MDISP_FONT8x6_VERTICAL_EN

		#endif // MDISP_FONT8x6_EN

		#ifdef MDISP_FONT16x10_EN
			void text_16x10_putc(uint8_t c);
			void text_16x10_puts(char* s);
			void text_16x10_new_line(void);

		#endif // MDISP_FONT16x10_EN

		#ifdef MDISP_FONT32x24_EN
			void text_32x24_out(uint8_t c, int16_t x, int16_t y, bool flush_forrce);
		#endif  // MDISP_FONT32x24_EN

		// fields
		#ifdef MDISP_DEFERRED_FLUSH
			bool m_FlushImmediate;
		#endif  // MDISP_DEFERRED_FLUSH
		uint8_t m_OverwriteMode;

	private:
		// methods

		char send_data_m(uint8_t data, uint8_t mask);
		char send_data(uint8_t data);

		#ifdef MDISP_GRAPHIC_EN
			#ifdef MDISP_BEZIER_MAXP
				int32_t interpolate_16_16_by_2_30(int32_t v1, int32_t v2, uint32_t factor);
			#endif  // MDISP_BEZIER_MAXP
//			#ifdef MDISP_GRAPH_TRIANGLE_EN
//				int32_t get_incline_16p16(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
//			#endif
		#endif  // MDISP_GRAPHIC_EN

		//fields

		MdispDevice* m_disp;

		int16_t m_x, m_y8;
		int16_t width1, height1, height8;
		#ifdef MDISP_BUFF_EN
			uint8_t * m_buff;
			int     m_buff_size;
			#ifdef MDISP_DEFERRED_FLUSH
				uint8_t * m_dirty_buff;
				int       m_dirty_buff_size;
				void flush_segment(int16_t x0, int16_t x1, int16_t y8);
			#endif
		#endif

};


#endif
