#include <wx/wx.h>

#include <wx/sizer.h>
#include <wx/gbsizer.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/wrapsizer.h>
#include <wx/generic/stattextg.h>

#include "zwx_helper.h"
using namespace zhelper::wxWidgets;

/** +++++++++++++++++++++++++++++++++++++++++++++++++ */

enum
{
    LAYOUT_TEST_SIZER = 101,
    LAYOUT_TEST_NB_SIZER,
    LAYOUT_TEST_GB_SIZER,
    LAYOUT_TEST_PROPORTIONS,
    LAYOUT_TEST_SET_MINIMAL,
    LAYOUT_TEST_NESTED,
    LAYOUT_TEST_WRAP,
    LAYOUT_QUIT = wxID_EXIT,
    LAYOUT_ABOUT = wxID_ABOUT
};

class MyFrame : public wxFrame
{
public:
    MyFrame() : wxFrame(NULL, wxID_ANY, wxString("")) {} 
    void TestMenuCommand(wxCommandEvent& event)
    {
        wxMessageBox("handled by MyFrame", event.GetString());
    }
    wxDECLARE_EVENT_TABLE();
};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
wxEND_EVENT_TABLE()

class MyFrameDelegate
{
public:
    MyFrameDelegate(wxFrame* frame) : frame_(frame) {}
    void TestMenuCommand(wxCommandEvent& event)
    {
        wxMessageBox("handled by MyFrameDelegate", event.GetString());
    }
    void OnClick(wxCommandEvent& event)
    {
        wxMessageBox("handled by MyFrameDelegate", "OnClick Button");
    }
private:
    wxFrame* frame_;
};

class MyApp : public wxApp
{
public:
    MyApp() {}
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

std::shared_ptr<MyFrameDelegate> sptr_delegate;

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    
    MyFrame* frame = new MyFrame;
    sptr_delegate = std::shared_ptr<MyFrameDelegate>(new MyFrameDelegate(frame));
    MyFrameDelegate* delegate = sptr_delegate.get();
    wxPanel* p = new wxPanel((wxWindow*)frame, wxID_ANY);
    wxMenuBar* mb = 
    menu::begin(new wxMenuBar)
                ("&File",
                 menu::begin(new wxMenu)
                    (LAYOUT_TEST_PROPORTIONS, "&Proportions demo...\tF1", &MyFrame::TestMenuCommand, frame)
                    (LAYOUT_TEST_SIZER, "Test wx&FlexSizer...\tF2", &MyFrame::TestMenuCommand, frame)
                    (LAYOUT_TEST_NB_SIZER, "Test &notebook sizers...\tF3", &MyFrame::TestMenuCommand, frame)
                    (LAYOUT_TEST_GB_SIZER, "Test &gridbag sizer...\tF4", frame, [=](wxCommandEvent& event) { frame->TestMenuCommand(event); })
                    (LAYOUT_TEST_SET_MINIMAL, "Test Set&ItemMinSize...\tF5")
                    (LAYOUT_TEST_NESTED, "Test nested sizer in a wxPanel...\tF6", frame, [=](wxCommandEvent& event) { sptr_delegate->TestMenuCommand(event); })(LAYOUT_TEST_WRAP, "Test wrap sizers...\tF7")
                    (menu::end))
                ("&Help",
                 menu::begin(new wxMenu)
                    (LAYOUT_ABOUT, "&About", "About layout demo...")
                    (menu::end))
                ("level-tree",
                 menu::begin(new wxMenu)
                    (wxID_ANY, "level-1.1", 
                        menu::begin(new wxMenu)
                                (wxID_ANY, "level-1.1-level-2.1")
                                (wxID_ANY, "level-1.1-level-2.2")
                                (menu::end))
                    (wxID_ANY, "level-1.2", 
                        menu::begin(new wxMenu)
                                (wxID_ANY, "level-1.2-level-2.1")
                                (wxID_ANY, "level-1.2-level-2.2")
                                (menu::end))
                    (menu::end))
                (menu::end, 
                    [=](wxMenuBar* mb) {
                        frame->SetMenuBar(mb);
                    });
    
    layout::begin(new wxBoxSizer(wxVERTICAL))
    // 1) top: create wxStaticText with minimum size equal to its default size
        (new wxStaticText( p, wxID_ANY, "An explanation (wxALIGN_RIGHT)." ),
        wxSizerFlags().Align(wxALIGN_RIGHT).Border(wxALL & ~wxBOTTOM, 5))
        (new wxStaticText( p, wxID_ANY, "An explanation (wxALIGN_LEFT)." ),
        wxSizerFlags().Align(wxALIGN_LEFT).Border(wxALL & ~wxBOTTOM, 5))
        (new wxStaticText( p, wxID_ANY, "An explanation (wxALIGN_CENTRE_HORIZONTAL)." ),
        wxSizerFlags().Align(wxALIGN_CENTRE_HORIZONTAL).Border(wxALL & ~wxBOTTOM, 5))

    // 2) top: create wxTextCtrl with minimum size (100x60)
        (new wxTextCtrl( p, wxID_ANY, "My text (wxEXPAND).", wxDefaultPosition, wxSize(100,60), wxTE_MULTILINE),
        wxSizerFlags(1).Expand().Border(wxALL, 5))

    // 2.5) Gratuitous test of wxStaticBoxSizers
        (layout::begin(new wxStaticBoxSizer(new wxStaticBox(p, wxID_ANY, "A wxStaticBoxSizer"), wxVERTICAL))
            (new wxStaticText(p, wxID_ANY, "And some TEXT inside it"), wxSizerFlags().Border(wxALL, 30))
            (layout::end), wxSizerFlags(1).Expand().Border(wxALL, 10))

    // 2.7) And a test of wxGridSizer
        (layout::begin(new wxGridSizer(2, 5, 5))
                (new wxStaticText(p, wxID_ANY, "Label"),
                wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL))
                (new wxTextCtrl(p, wxID_ANY, "Grid sizer demo"),
                wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL))
                (new wxStaticText(p, wxID_ANY, "Another label"),
                wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL))
                (new wxTextCtrl(p, wxID_ANY, "More text"),
                wxSizerFlags(1).Align(wxGROW | wxALIGN_CENTER_VERTICAL))
                (new wxStaticText(p, wxID_ANY, "Final label"),
                wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL))
                (new wxTextCtrl(p, wxID_ANY, "And yet more text"),
                wxSizerFlags().Align(wxGROW | wxALIGN_CENTER_VERTICAL))
                (layout::end), wxSizerFlags().Proportion(1).Expand().Border(wxALL, 10))


#if wxUSE_STATLINE
    // 3) middle: create wxStaticLine with minimum size (3x3)
        (new wxStaticLine( p, wxID_ANY, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
            wxSizerFlags().Expand())
#endif // wxUSE_STATLINE


    // 4) bottom: create two centred wxButtons
        (layout::begin(new wxBoxSizer( wxHORIZONTAL ))
            (new wxButton( p, wxID_ANY, "Two buttons in a box" ),
            wxSizerFlags().Border(wxALL, 7))[onclick = [=](wxCommandEvent& e) { delegate->OnClick(e); } ]
            (new wxButton( p, wxID_ANY, "(wxCENTER)" ),
            wxSizerFlags().Border(wxALL, 7))
            (layout::end), wxSizerFlags().Center())
        (layout::end, 
            [=](wxSizer* s) { 
                p->SetSizer(s); 
                s->SetSizeHints(frame); 
            });
    
    frame->Show(true);
    return true;
}
