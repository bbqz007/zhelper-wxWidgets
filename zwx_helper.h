/**
MIT License

Copyright (c) 2020 bbqz007 <https://github.com/bbqz007, http://www.cnblogs.com/bbqzsl>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ZWX_HELPER__H_
#include <functional>
#include <memory>

namespace zhelper
{
namespace wxWidgets
{

template<class T, int tag>
class onevent
{
public:
    typedef onevent<T, tag> this_type;
    onevent() {}
    onevent(const std::function<void(T&)>& f) : functor_(f) {}
    onevent operator = (const std::function<void(T&)>& f)
    {
        return this_type(f);
    }
    onevent operator , (const std::function<void(T&)>& f)
    {
        return this_type(f);
    }
    onevent operator () (const std::function<void(T&)>& f)
    {
        functor_ = f;
        return this_type(f);
    }
    std::function<void(T&)> functor_;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
typedef onevent<wxKeyEvent, 'kyup'>  Fonkeyup; Fonkeyup onkeyup;
typedef onevent<wxKeyEvent, 'kydn'>  Fonkeydown; Fonkeydown onkeydown;
typedef onevent<wxCommandEvent, 'clck'> Fonclick; Fonclick onclick;
typedef onevent<wxDropFilesEvent, 'drpf'> Fondropfiles; Fondropfiles ondropfiles;
#pragma GCC diagnostic pop




class layout_end {};
class menu_end {};

#define NO_IMPL(F) virtual void operator ()(F) {}
class prop_builder_itf
{
public:
    prop_builder_itf() :
        sf_(0), deferred_(false) {}
    virtual ~prop_builder_itf() {}
    virtual void operator ()(wxSizerFlags) = 0;
    virtual void operator ()(wxSizer*) = 0;
    virtual void operator ()(long) = 0;
    // wxWindoow
    virtual void operator ()(const Fonkeyup&) = 0;
    virtual void operator ()(const Fonkeydown&) = 0;
    virtual void operator ()(const Fonclick&) = 0;
    virtual void operator ()(const Fondropfiles&) = 0;
    // wxSizer
    wxSizerFlags sf_;
    bool deferred_;
};

class no_impl_prop_builder_base : public prop_builder_itf
{
public:
    void operator ()(wxSizerFlags sf)
    {
        sf_ = sf;
    }
    NO_IMPL(long)
    NO_IMPL(const Fonkeyup&)
    NO_IMPL(const Fonkeydown&)
    NO_IMPL(const Fonclick&)
    NO_IMPL(const Fondropfiles&)
};

template<class T>
class prop_builder_base : public no_impl_prop_builder_base
{
public:
    prop_builder_base(T* t) : t_(t) {}
    virtual void operator ()(wxSizer* s)
    {
        if (!deferred_)
            return;
        static const wxSizerFlags zf;
        if (0 == memcmp(&sf_, &zf, sizeof(zf)))
            s->Add(t_);
        else
            s->Add(t_, sf_);
    }
    virtual void operator ()(const Fonkeyup& f)
    {
        t_->Bind(wxEVT_KEY_UP, f.functor_);
    }
    virtual void operator ()(const Fonkeydown& f)
    {
        t_->Bind(wxEVT_KEY_DOWN, f.functor_);
    }
    T* t_;
};

template<>
class prop_builder_base<wxSizer> : public no_impl_prop_builder_base
{
public:
    prop_builder_base(wxSizer* t) : t_(t) {}
    virtual void operator ()(wxSizer* s)
    {
        if (!deferred_)
            return;
        static const wxSizerFlags zf;
        if (0 == memcmp(&sf_, &zf, sizeof(zf)))
            s->Add(t_);
        else
            s->Add(t_, sf_);
    }
    wxSizer* t_;
};

template<class T>
class prop_builder : public prop_builder_base<T>
{
public:
    prop_builder(T* t) : prop_builder_base<T>(t) {}
};

template<>
class prop_builder<wxButton> : public prop_builder_base<wxButton>
{
public:
    prop_builder(wxButton* t) : prop_builder_base<wxButton>(t) {}
    virtual void operator ()(const Fonclick& f)
    {
        t_->Bind(wxEVT_BUTTON, f.functor_);
    }
};

template<>
class prop_builder<wxTextCtrl> : public prop_builder_base<wxTextCtrl>
{
public:
    prop_builder(wxTextCtrl* t) : prop_builder_base<wxTextCtrl>(t) {}
    virtual void operator ()(const Fondropfiles& f)
    {
        t_->DragAcceptFiles(true);
        t_->Bind(wxEVT_DROP_FILES, f.functor_);
    }
    virtual void operator ()(long style)
    {
        t_->SetWindowStyleFlag(style);
    }
};

#define DELCARE_SIZER_PROP_BUILDER(T) \
template<>  \
class prop_builder<T> : public prop_builder_base<wxSizer> \
{ \
public: \
    prop_builder(T* t) : prop_builder_base<wxSizer>(t) {} \
};
DELCARE_SIZER_PROP_BUILDER(wxBoxSizer)
DELCARE_SIZER_PROP_BUILDER(wxGridSizer)
DELCARE_SIZER_PROP_BUILDER(wxWrapSizer)
DELCARE_SIZER_PROP_BUILDER(wxStaticBoxSizer)
#undef DELCARE_SIZER_PROP_BUILDER

#undef NO_IMPL

template<class T>
class builder
{
public:
    static builder& begin(T* t) { return * new builder(t); }
    T* t_;
    wxSizerFlags sf_;
    builder(T* t) : t_(t) {}
    template<class Y>
    builder& operator () (Y* y)
    {
        return operator ()(y, y);
    }
    template<class Y>
    builder& operator () (Y* y, wxSizerFlags& flags)
    {
        return operator ()(y, y, flags);
    }
    template<class Y>
    builder& operator () (std::pair<Y*, wxSizerFlags> ysf)
    {
        Y* y = ysf.first;
        wxSizerFlags& flags = ysf.second;
        static const wxSizerFlags zf;
        if (0 == memcmp(&flags, &zf, sizeof(zf)))
            return operator ()(y, y);
        return operator ()(y, y, flags);
    }

    builder& operator [] (long style)
    {
        (*prop_)(style);
        return *this;
    }

    builder& operator [] (const Fonclick& f)
    {
        (*prop_)(f);
        return *this;
    }

    builder& operator [] (const Fondropfiles& f)
    {
        (*prop_)(f);
        return *this;
    }

    builder& operator [] (const wxSizerFlags& sf)
    {
        if (prop_)
            (*prop_)(sf);
        else
            sf_ = sf;
        return *this;
    }

    std::pair<T*, wxSizerFlags> operator() (layout_end&, void (*complete)(T*) = NULL)
    {
        finish_prev_deferred();
        T* ret = t_;
        std::pair<T*, wxSizerFlags> pack = std::make_pair(t_, sf_);
        if (complete)
            complete(ret);
        delete this;
        return pack;
    }
    std::pair<T*, wxSizerFlags> operator() (layout_end&, void (*complete)(wxSizer*))
    {
        finish_prev_deferred();
        T* ret = t_;
        std::pair<T*, wxSizerFlags> pack = std::make_pair(t_, sf_);
        if (complete)
            complete(ret);
        delete this;
        return pack;
    }
    std::pair<T*, wxSizerFlags> operator() (layout_end&, std::function<void(wxSizer*)> complete)
    {
        finish_prev_deferred();
        T* ret = t_;
        std::pair<T*, wxSizerFlags> pack = std::make_pair(t_, sf_);
        if (complete)
            complete(ret);
        delete this;
        return pack;
    }

private:
    template<class Y>
    builder& operator () (Y* y, wxWindow*)
    {
        finish_prev_deferred();
        prop_.reset((prop_builder_itf*)new prop_builder<Y>(y));
        //t_->Add(y);
        prop_->deferred_ = true;
        return *this;
    }
    template<class Y>
    builder& operator () (Y* y, wxWindow*, wxSizerFlags& flags)
    {
        finish_prev_deferred();
        prop_.reset((prop_builder_itf*)new prop_builder<Y>(y));
        t_->Add(y, flags);
        return *this;
    }
    template<class Y>
    builder& operator () (Y* y, wxSizer*)
    {
        finish_prev_deferred();
        prop_.reset((prop_builder_itf*)new prop_builder<Y>(y));
        //t_->Add(y);
        prop_->deferred_ = true;
        return *this;
    }
    template<class Y>
    builder& operator () (Y* y, wxSizer*, wxSizerFlags& flags)
    {
        finish_prev_deferred();
        prop_.reset((prop_builder_itf*)new prop_builder<Y>(y));
        t_->Add(y, flags);
        return *this;
    }
    void finish_prev_deferred()
    {
        if (!prop_)
            return;
        (*prop_)(t_);
    }
    std::shared_ptr<prop_builder_itf> prop_;
};


template<class T>
class menu_builder
{
public:
    //typedef wxMenu T;
    static menu_builder& begin(T* t) { return * new menu_builder(t); }
    T* t_;
    int id_;
    menu_builder(T* t) : t_(t), id_(wxID_ANY) {}
    template<class Y>
    menu_builder& operator () (Y* y)
    {
        t_->Append(y);
        return *this;
    }
    menu_builder& operator () (int itemid,
                            const wxString& text = wxEmptyString,
                            const wxString& help = wxEmptyString,
                            wxItemKind kind = wxITEM_NORMAL)
    {
        id_ = itemid;
        t_->Append(itemid, text, help, kind);
        return *this;
    }
    template<typename Functor>
    menu_builder& operator () (int itemid,
                            const wxString& text,
                            wxEvtHandler* delegate,
                            Functor functor
                              )
    {
        id_ = itemid;
        t_->Append(itemid, text, wxEmptyString, wxITEM_NORMAL);
        delegate->Bind(wxEVT_MENU, functor, itemid);
        return *this;
    }
    menu_builder& operator () (int itemid,
                            const wxString& text,
                            wxEvtHandler* delegate,
                            std::function<void(wxCommandEvent&)> functor
                              )
    {
        id_ = itemid;
        t_->Append(itemid, text, wxEmptyString, wxITEM_NORMAL);
        delegate->Bind(wxEVT_MENU, functor, itemid);
        return *this;
    }
    menu_builder& operator () (int itemid,
                            const wxString& text,
                            std::function<void(wxCommandEvent&)> functor
                              )
    {
        id_ = itemid;
        t_->Append(itemid, text, wxEmptyString, wxITEM_NORMAL);
        t_->Bind(wxEVT_MENU, functor, itemid);
        return *this;
    }
    template<typename Class, typename EventArg, typename EventHandler>
    menu_builder& operator () (int itemid,
                               const wxString& text,
                                void (Class::*method)(EventArg &),
                                EventHandler *handler)
    {
        id_ = itemid;
        t_->Append(itemid, text, wxEmptyString, wxITEM_NORMAL);
        handler->Bind(wxEVT_MENU, method, handler, itemid);
        return *this;
    }
    menu_builder& operator () (int itemid,
                          const wxString& text,
                          wxMenu* sub)
    {
        id_ = itemid;
        t_->Append(itemid, text, sub);
        return *this;
    }
    menu_builder& operator () (wxMenu* sub,
                          const wxString& text)
    {
        id_ = wxID_ANY;
        t_->Append(sub, text);
        return *this;
    }
    menu_builder& operator () (const wxString& text,
                               wxMenu* sub)
    {
        id_ = wxID_ANY;
        t_->Append(sub, text);
        return *this;
    }
    menu_builder& operator [] (const Fonclick& f)
    {
        t_->Bind(wxEVT_MENU, f.functor_, id_);
        return *this;
    }
    menu_builder& operator [] (const std::function<void(wxCommandEvent&)> functor)
    {
        t_->Bind(wxEVT_MENU, functor, id_);
        return *this;
    }
    template<typename Functor>
    menu_builder& operator [] (Functor functor)
    {
        t_->Bind(wxEVT_MENU, functor, id_);
        return *this;
    }
    T* operator() (menu_end&, void(*complete)(T*) = NULL)
    {
        T* ret = t_;
        if (complete)
            complete(t_);
        delete this;
        return ret;
    }
    T* operator() (menu_end&, const std::function<void(T*)>& complete)
    {
        T* ret = t_;
        if (complete)
            complete(t_);
        delete this;
        return ret;
    }
};

class menu
{
public:
    static menu_end end;
    template<class T>
    static menu_builder<T>& begin(T* t) { return * new menu_builder<T>(t);  }
};
menu_end menu::end;

class layout
{
public:
    static layout_end end;
    static std::function<void(wxSizer*)> oncomplete;
    template<class T>
    static builder<T>& begin(T* t) { return * new builder<T>(t);  }
};
layout_end layout::end;
std::function<void(wxSizer*)> layout::oncomplete;

} // end namespace wxWidgets

} // end namespace zhelper

#endif // end ZWX_HELPER__H_
