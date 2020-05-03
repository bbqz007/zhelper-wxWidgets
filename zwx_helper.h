
#ifndef ZWX_HELPER__H_
#include <functional>
#include <memory>

namespace zhelper 
{
namespace wxWidgets
{

template<class T, int>
class onevent
{
public:
    onevent& operator = (const std::function<void(T&)>& f)
    {
        functor_ = f;
        return *this;
    }
    onevent& operator () (std::function<void(T&)>& f)
    {
        functor_ = f;
        return *this;
    } 
    std::function<void(T&)> functor_;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
typedef onevent<wxKeyEvent, 'kyup'>  Fonkeyup; Fonkeyup onkeyup;
typedef onevent<wxKeyEvent, 'kydn'>  Fonkeydown; Fonkeydown onkeydown;
typedef onevent<wxCommandEvent, 'clck'> Fonclick; Fonclick onclick;
#pragma GCC diagnostic pop




class layout_end {};
class menu_end {};

class prop_builder_itf
{
public:
    virtual ~prop_builder_itf() {}
    virtual void operator ()(Fonkeyup&) = 0;
    virtual void operator ()(Fonkeydown&) = 0;
    virtual void operator ()(Fonclick&) = 0;
};

template<class T>
class prop_builder_base : public prop_builder_itf
{
public:
    prop_builder_base(T* t) : t_(t) {}
    virtual void operator ()(Fonkeyup& f)
    {
        t_->Bind(wxEVT_KEY_UP, f.functor_);
    }
    virtual void operator ()(Fonkeydown& f)
    {
        t_->Bind(wxEVT_KEY_DOWN, f.functor_);
    }
    virtual void operator ()(Fonclick& f) {}
    T* t_;
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
    virtual void operator ()(Fonclick& f)
    {
        t_->Bind(wxEVT_BUTTON, f.functor_);
    }
};
    
    

template<class T>
class builder
{
public:
    static builder& begin(T* t) { return * new builder(t); }
    T* t_;
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
    
    builder& operator [] (Fonclick& f)
    {
        (*prop_)(f);
        return *this;
    }
    
    T* operator() (layout_end&, void (*complete)(T*) = NULL) 
    {
        T* ret = t_;
        if (complete)
            complete(ret);
        delete this;
        return ret;
    }
    T* operator() (layout_end&, void (*complete)(wxSizer*)) 
    {
        T* ret = t_;
        if (complete)
            complete(ret);
        delete this;
        return ret;
    }
    T* operator() (layout_end&, std::function<void(wxSizer*)> complete) 
    {
        T* ret = t_;
        if (complete)
            complete(ret);
        delete this;
        return ret;
    }
    
private:
    template<class Y>
    builder& operator () (Y* y, wxWindow*)
    {
        prop_.reset((prop_builder_itf*)new prop_builder<Y>(y));
        t_->Add(y);
        return *this;
    }
    template<class Y>
    builder& operator () (Y* y, wxWindow*, wxSizerFlags& flags)
    {
        prop_.reset((prop_builder_itf*)new prop_builder<Y>(y));
        t_->Add(y, flags);
        return *this;
    }
    template<class Y>
    builder& operator () (Y* y, wxSizer*)
    {
        //prop_.reset((prop_builder_itf*)new prop_builder<Y>));
        t_->Add(y);
        return *this;
    }
    template<class Y>
    builder& operator () (Y* y, wxSizer*, wxSizerFlags& flags)
    {
        //prop_.reset((prop_builder_itf*)new prop_builder<Y>));
        t_->Add(y, flags);
        return *this;
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
    menu_builder(T* t) : t_(t) {}
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
        t_->Append(itemid, text, wxEmptyString, wxITEM_NORMAL);
        delegate->Bind(wxEVT_MENU, functor, itemid);
        return *this;
    }
    menu_builder& operator () (int itemid,
                            const wxString& text,
                            std::function<void(wxCommandEvent&)> functor
                              )
    {
        static wxButton delegate;
        t_->Append(itemid, text, wxEmptyString, wxITEM_NORMAL);
        delegate.Bind(wxEVT_MENU, functor, itemid);
        return *this;
    }
    template<typename Class, typename EventArg, typename EventHandler>
    menu_builder& operator () (int itemid,
                               const wxString& text,
                                void (Class::*method)(EventArg &),
                                EventHandler *handler)
    {
        t_->Append(itemid, text, wxEmptyString, wxITEM_NORMAL);
        handler->Bind(wxEVT_MENU, method, handler, itemid);
        return *this;
    }
    menu_builder& operator () (int itemid,
                          const wxString& text,
                          wxMenu* sub)
    {
        t_->Append(itemid, text, sub);
        return *this;
    }
    menu_builder& operator () (wxMenu* sub,
                          const wxString& text)
    {
        t_->Append(sub, text);
        return *this;
    }
    menu_builder& operator () (const wxString& text, 
                               wxMenu* sub)
    {
        t_->Append(sub, text);
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
    T* operator() (menu_end&, std::function<void(T*)> complete) 
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
    template<class T>
    static builder<T>& begin(T* t) { return * new builder<T>(t);  }
};
layout_end layout::end;

} // end namespace wxWidgets

} // end namespace zhelper

#endif // end ZWX_HELPER__H_
