/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: onPanel */
#define  PANEL_GRAPH_FILTERED             2       /* control type: graph, callback function: (none) */
#define  PANEL_HISTOGRAM                  3       /* control type: graph, callback function: (none) */
#define  PANEL_GRAPH_RAW                  4       /* control type: graph, callback function: (none) */
#define  PANEL_NEXT                       5       /* control type: command, callback function: onNEXT */
#define  PANEL_SAVE                       6       /* control type: command, callback function: SAVE_CALLBACK */
#define  PANEL_LOAD                       7       /* control type: command, callback function: LOAD_CALLBACK */
#define  PANEL_APLICA                     8       /* control type: command, callback function: APLICA_CALLBACK */
#define  PANEL_ENVELOPE                   9       /* control type: command, callback function: ENVELOPE_CALLBACK */
#define  PANEL_SWITCH_PANELS              10      /* control type: command, callback function: SWITCH_CALLBACK */
#define  PANEL_DERIVATE                   11      /* control type: command, callback function: DERIVATE_CALLBACK */
#define  PANEL_PREV                       12      /* control type: command, callback function: onPREV */
#define  PANEL_SKEWNESS                   13      /* control type: numeric, callback function: (none) */
#define  PANEL_KURTOSIS                   14      /* control type: numeric, callback function: (none) */
#define  PANEL_ZEROS                      15      /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIAN                     16      /* control type: numeric, callback function: (none) */
#define  PANEL_DISPERSION                 17      /* control type: numeric, callback function: (none) */
#define  PANEL_MAX_INDEX                  18      /* control type: numeric, callback function: (none) */
#define  PANEL_MEAN                       19      /* control type: numeric, callback function: (none) */
#define  PANEL_MAX                        20      /* control type: numeric, callback function: (none) */
#define  PANEL_MIN_INDEX                  21      /* control type: numeric, callback function: (none) */
#define  PANEL_MIN                        22      /* control type: numeric, callback function: (none) */
#define  PANEL_START_DISP                 23      /* control type: numeric, callback function: (none) */
#define  PANEL_END_DISP                   24      /* control type: numeric, callback function: (none) */
#define  PANEL_DECORATION_2               25      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_3               26      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION                 27      /* control type: deco, callback function: (none) */
#define  PANEL_RING                       28      /* control type: ring, callback function: (none) */
#define  PANEL_ALPHA                      29      /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIERE                    30      /* control type: numeric, callback function: (none) */

#define  PANEL_2                          2       /* callback function: onPanel2 */
#define  PANEL_2_NEXT_3                   2       /* control type: command, callback function: onNEXT_3 */
#define  PANEL_2_PREV                     3       /* control type: command, callback function: onPREV_2 */
#define  PANEL_2_NEXT                     4       /* control type: command, callback function: onNEXT_2 */
#define  PANEL_2_GRAPH_FILTER_WINDOW      5       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_RAW_WINDOW         6       /* control type: graph, callback function: (none) */
#define  PANEL_2_SAVE                     7       /* control type: command, callback function: SAVE_CALLBACK_2 */
#define  PANEL_2_GRAPH_SPECTRUM_FILTER    8       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_FILTER             9       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_SPECTRU            10      /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_RAW                11      /* control type: graph, callback function: (none) */
#define  PANEL_2_SWITCH_PANELS_1          12      /* control type: command, callback function: SWITCH_CALLBACK */
#define  PANEL_2_START_DISP_2             13      /* control type: numeric, callback function: (none) */
#define  PANEL_2_START_DISP               14      /* control type: numeric, callback function: (none) */
#define  PANEL_2_END_DISP                 15      /* control type: numeric, callback function: (none) */
#define  PANEL_2_DECORATION_4             16      /* control type: deco, callback function: (none) */
#define  PANEL_2_DECORATION_3             17      /* control type: deco, callback function: (none) */
#define  PANEL_2_PREV_3                   18      /* control type: command, callback function: onPREV_3 */
#define  PANEL_2_RING_3                   19      /* control type: ring, callback function: (none) */
#define  PANEL_2_RING_4                   20      /* control type: ring, callback function: Set0 */
#define  PANEL_2_RING                     21      /* control type: ring, callback function: onRING */
#define  PANEL_2_RING_2                   22      /* control type: ring, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK APLICA_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DERIVATE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ENVELOPE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LOAD_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onNEXT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onNEXT_2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onNEXT_3(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onPanel2(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onPREV(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onPREV_2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onPREV_3(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onRING(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SAVE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SAVE_CALLBACK_2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Set0(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SWITCH_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif