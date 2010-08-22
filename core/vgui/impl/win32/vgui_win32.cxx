// This is core/vgui/impl/win32/vgui_win32.cxx
#include "vgui_win32.h"

#include <vgui/vgui_gl.h> // for glFlush()
#include <vcl_iostream.h> // for vcl_cerr
#include <vcl_cassert.h> // for assert
#include <vcl_cstring.h> // for vcl_strlen()
#include "vgui_win32_window.h"
#include "vgui_win32_dialog_impl.h"

vgui_win32* vgui_win32::_instance = 0;

// Return a single instance of vgui_win32.
vgui_win32* vgui_win32::instance()
{
  if ( _instance == 0 )
    _instance = new vgui_win32();

  return _instance;
}

vgui_win32::vgui_win32()
{
  _hInstance = GetModuleHandle(NULL);
  _hPrevInstance = NULL;
  _szCmdLine = NULL;
  _iCmdShow = SW_SHOW;

  _szAppName = NULL;

  windows_to_delete.clear();
  current_window = NULL;
  dialogs_to_delete.clear();
  current_dialog = NULL;
}

vgui_win32::~vgui_win32()
{
}


// Process command line arguments
BOOL vgui_win32::ProcessShellCommand(int argc, char **argv)
{
  // We set _szAppName to be the app name.
  char *p, *q = argv[0];

  // Skip path
  while ( p = vcl_strchr(q, '\\') ) q = ++p;
  // remove ".exe" suffix
  p = vcl_strstr(q, ".exe");
  // Now q points to the app name.
  if (p)
    _szAppName = (char *)malloc(sizeof(char)*(p-q+1));
  else // .exe is not provided.
    _szAppName = (char *)malloc(sizeof(char)*(1+vcl_strlen(q)));

  if ( _szAppName == NULL )
    return FALSE;
  if (p) *p = 0;
  vcl_strcpy(_szAppName, q);

  // Convert argv into _szCmdLine
  _szCmdLine = GetCommandLine();

  return TRUE;
}

void vgui_win32::init(int &argc, char **argv)
{
  ProcessShellCommand(argc, argv);

  WNDCLASS wndclass;
  wndclass.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = globalWndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = _hInstance;
  wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName  = _szAppName;
  wndclass.lpszClassName = _szAppName;

  if ( !RegisterClass(&wndclass) ) {
    vcl_cerr << "Fail to register window class for main window.\n";
    assert(false); // quit ugly
  }

  // Register a window class for dialog box.
  wndclass.style         = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = globalDialogProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = _hInstance;
  wndclass.hIcon         = LoadIcon(_hInstance, IDI_APPLICATION);
  wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
  wndclass.lpszMenuName  = NULL;
  wndclass.lpszClassName = TEXT("vgui_win32_dialog");

  if ( !RegisterClass(&wndclass) ) {
    MessageBox(NULL, TEXT("Fail to register window class for dialog box!"),
               NULL, MB_ICONERROR);
    assert(false); // quit ugly
  }

  // Register a window class for inline tableau control
  wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wndclass.lpfnWndProc   = globalTableauProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = _hInstance;
  wndclass.hIcon         = LoadIcon(_hInstance, IDI_APPLICATION);
  wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wndclass.lpszMenuName  = NULL;
  wndclass.lpszClassName = TEXT("vgui_win32_inline_tab");

  if ( !RegisterClass(&wndclass) ) {
    MessageBox(NULL, TEXT("Fail to register window class for inline tableau control!"),
               NULL, MB_ICONERROR);
    assert(false); // quit ugly
  }
}

void vgui_win32::uninit()
{
  for (unsigned i=0; i<windows_to_delete.size(); i++)
    delete windows_to_delete[i];

  // dialogs are already freed at the end of scope of caller function.
}

vgui_window* vgui_win32::produce_window(int width, int height,
                                        vgui_menu const &menubar,
                                        char const *title)
{
  vgui_window* a_window = new vgui_win32_window(_hInstance, _szAppName, width, height, menubar, title);
  windows_to_delete.push_back(a_window);
  current_window = a_window;
  return a_window;
}

vgui_window* vgui_win32::produce_window(int width, int height,
                                        char const *title)
{
  vgui_window* a_window = new vgui_win32_window(_hInstance, _szAppName, width, height, title);
  windows_to_delete.push_back(a_window);
  current_window = a_window;
  return a_window;
}

vgui_dialog_impl* vgui_win32::produce_dialog(char const *name)
{
  vgui_window *win = get_current_window();

  current_dialog = new vgui_win32_dialog_impl(name,
                                              win ? ((vgui_win32_window*)win)->getWindowHandle() : NULL);
  dialogs_to_delete.push_back(current_dialog);
  return current_dialog;
}

vgui_dialog_extensions_impl* vgui_win32::produce_dialog_extension(char const *name)
{
  //return new vgui_win32_dialog_extension_impl(name);
  return 0;
}

// Run the event loop. Windows uses messages
void vgui_win32::run()
{
  MSG msg;

  while ( GetMessage(&msg, NULL, 0, 0) ) {
    vgui_win32_window *pwin = (vgui_win32_window*)get_current_window();
    if (pwin) {
      HWND hwnd = pwin->getWindowHandle();
      HACCEL hAccel = pwin->getAccelHandle();
      if ( !(hAccel && TranslateAccelerator(hwnd, hAccel, &msg)) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    else {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

// TODO: This function is not called yet.
void vgui_win32::run_one_event()
{
  if ( !PumpMessage() )
    PostQuitMessage(0);
}

// TODO: This function is not called yet.
void vgui_win32::run_till_idle()
{
  MSG msg;

  while ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ) {
    if ( !PumpMessage() ) {
      PostQuitMessage(0);
      break;
    }
  }
}

// TODO: This function is not called yet.
// Remove all events from the event queue. Copy the code from that of
// counterparts such as mfc, gkt2.
void vgui_win32::flush()
{
  glFlush();
  run_till_idle();
}

// TODO: This function is not called yet.
// Add event to the event queue.
void vgui_win32::add_event(vgui_event const &e)
{
  //PostMessage(); // TODO
}

// Quit application
void vgui_win32::quit()
{
  PostQuitMessage(0);
}

// TODO: This function is not called yet.
// Pump a message from the thread's message queue and process it.
BOOL vgui_win32::PumpMessage()
{
  MSG msg;

  if ( !GetMessage(&msg, NULL, 0, 0) )
    return FALSE;

  TranslateMessage(&msg);
  DispatchMessage(&msg);

  return TRUE;
}

// Find the window whose handle is hwnd.
inline int vgui_win32::find_window(HWND hwnd)
{
  int i;
  vcl_vector<vgui_window*>::const_iterator it;
  for ( i = 0,  it = windows_to_delete.begin();
        it != windows_to_delete.end(); it++, i++ ) {
    HWND the_hwnd = ((vgui_win32_window*)(*it))->getWindowHandle();
    if ( the_hwnd == hwnd )
      return i;
  }
  // Not found
  return -1;
}

inline void vgui_win32::dump_window_stack()
{
  vcl_cout << "z-top of window stack: ";

  vcl_vector<vgui_window*>::const_iterator it;
  for ( it = windows_to_delete.begin();
        it != windows_to_delete.end(); it++ )
    vcl_cout << ((vgui_win32_window*)(*it))->getWindowHandle() << ',';
  vcl_cout << vcl_endl;
}

// Set the current window as the one whose handle is "hwnd".
// In other words, raise the window hwnd to the top of z-order stack
// of windows.
void vgui_win32::set_current_window(HWND hwnd)
{
  int n = find_window(hwnd);
  if ( n == -1 || windows_to_delete[n] == current_window )
    return; // do nothing if hwnd is already top-most, or not found.

  current_window = windows_to_delete[n];
  windows_to_delete.erase(windows_to_delete.begin()+n);
  windows_to_delete.push_back(current_window);
  //dump_window_stack();
}


// Remove the current window from z-order stack of windows.
void vgui_win32::remove_current_window()
{
  windows_to_delete.pop_back();
  delete current_window;
  current_window = windows_to_delete.empty() ? NULL : windows_to_delete.back();
  //dump_window_stack();
}

void vgui_win32::remove_current_dialog()
{
  dialogs_to_delete.pop_back();
  current_dialog = dialogs_to_delete.empty() ? NULL : dialogs_to_delete.back();
}

LRESULT CALLBACK globalWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult = 0;
  vgui_win32_window* pwin;

  // Some messages should be handled here, not in vgui_window and vgui_adaptor

  vgui_win32::instance()->set_current_window(hwnd);

  if ( message == WM_DESTROY ) {
     // When multiple vgui_window objects are created, a WM_DESTROY message
     // is sent when one of the vgui_window is closed.
     // In this case, two operations have to be done. First, the current
     // window (along with device context and OpenGL rendering context)
     // should be switched. Second, the WM_DESTROY message should be
     // blocked unless no vgui_window object exists. Otherwise, the
     // application will quit since function PostQuitMessage() is called
     // (ie. WM_QUIT is sent) in response to WM_DESTROY message.

     vgui_win32::instance()->remove_current_window();

    // Call post_redraw to switch device context and GL rendering context.
    pwin = (vgui_win32_window*)vgui_win32::instance()->get_current_window();
    if ( pwin) pwin->get_adaptor()->post_redraw();

     // block WM_DESTROY message
     return 1;
  }

  pwin = (vgui_win32_window*)vgui_win32::instance()->get_current_window();
  if ( pwin )
    lResult = pwin->WndProc(hwnd, message, wParam, lParam);

  return lResult ? lResult : DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK globalDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT lResult = 0;
  vgui_win32_window* pwin;
  vgui_win32_dialog_impl *dlg;


  dlg = (vgui_win32_dialog_impl *)vgui_win32::instance()->get_current_dialog();
  if ( dlg )
    lResult = dlg->DialogProc(hDlg, message, wParam, lParam);

  if ( message == WM_DESTROY ) {
    // Call post_redraw to switch device context and GL rendering context.
    pwin = (vgui_win32_window*)vgui_win32::instance()->get_current_window();
    pwin->get_adaptor()->post_redraw();

    // Reset pointer to current dialog box.
    (vgui_win32::instance())->remove_current_dialog();
  }

  return lResult ? lResult : DefWindowProc(hDlg, message, wParam, lParam);
}

LRESULT CALLBACK globalTableauProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  vgui_win32_dialog_impl *dlg;

  // Get the dialog box the inline tableau belongs.
  dlg = (vgui_win32_dialog_impl *)vgui_win32::instance()->get_current_dialog();

  if ( dlg && dlg->get_inline_tableau_size() > 0 )
    dlg->get_current_tab()->OnCmdMsg(message, wParam, lParam);

  return DefWindowProc(hDlg, message, wParam, lParam);
}
