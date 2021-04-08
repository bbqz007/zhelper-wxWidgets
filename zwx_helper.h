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
#define ZWX_HELPER__H_
#include <functional>
#include <tuple>
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

#if __cplusplus < 201402L
template<typename F>
struct lambda_transfer_deduce
{
	lambda_transfer_deduce(const F& f) : f_(f)
	{

	}
	template<typename F, typename T, typename Y>
	T* operator () (const F& f, void(F::*)(T*) const, Y* y) const
	{
		return dynamic_cast<T*>(y);
	}
	template<typename F, typename T, typename X, typename Y>
	void operator () (const F& f, void(F::*)(T*) const, X* x, Y* y) const
	{
		T* t = this->operator()(f_, &F::operator(), x);
		if (!t)
			t = this->operator()(f_, &F::operator(), y);
		if (t)
			f_(t);
	}
	template<typename X, typename Y>
	void operator () (X* x, Y* y) const
	{
		this->operator()(f_, &F::operator(), x, y);
	}
	F f_;
};
#endif 

class lambda_transfer
{
public:
    // thanks to a solution of deduce types of arguments and result of lambda to make std::function
    //   by https://stackoverflow.com/questions/21245891/deduce-template-argument-when-lambda-passed-in-as-a-parameter
#if __cplusplus >= 201402L
    template<typename F>
    auto operator , (const F& f)
    {
        return operator ()(f, &F::operator());
    }
#else
	template<typename F>
	lambda_transfer_deduce<F> operator , (const F& f)
	{
		return lambda_transfer_deduce<F>(f);
	}
#endif
private:
    template<typename F, typename T>
    std::function<void(T*)> operator () (const F& f, void(F::*)(T*) const)
    {
        return std::function<void(T*)>(f);
    }
    template<typename F, typename ... Args>
    std::function<void(Args...)> operator () (const F& f, void(F::*)(Args...) const)
    {
        return std::function<void(Args...)>(f);
    }
};

class multievent_begin;
template<typename ... Types>
class multievent
{
public:
    multievent(Types... types)
    {
        tpl_ = std::make_tuple(types...);
    }
    multievent(const std::tuple<Types...>& tpl)
        : tpl_(tpl)
    {

    }
    template<typename T>
    multievent<Types..., T> operator , (const T& t)
    {
        traversal_assign(t.functor_);
        auto tpl = std::tuple_cat(tpl_, std::make_tuple(t));
        return multievent<Types..., T>(tpl);
    }
    template<size_t N, typename Tp, typename F>
    struct traversal_assign_act
    {
        static void act(Tp& tpl_, const F& f)
        {
            std::get<N>(tpl_).functor_ = f;
            traversal_assign_act<N-1, Tp, F>::act(tpl_, f);
        }
    };
    template<typename Tp, typename F>
    struct traversal_assign_act<0, Tp, F>
    {
        static void act(Tp&, const F& f)
        {

        }
    };
    template<typename T>
    void traversal_assign(const std::function<void(T&)>& f)
    {
        traversal_assign_act<sizeof...(Types) - 1,
                                std::tuple<Types...>&,
                                    const std::function<void(T&)>&>::act(tpl_, f);
    }
    template<size_t N, typename Tp, typename F>
    struct traversal_op_act
    {
        static void act(Tp& tpl_, F& f)
        {
            auto el = std::get<N>(tpl_);
            f(el);
            traversal_op_act<N-1, Tp, F>::act(tpl_, f);
        }
    };
    template<typename Tp, typename F>
    struct traversal_op_act<0, Tp, F>
    {
        static void act(Tp& tpl_, F& f)
        {

        }
    };
    template<typename F>
    void traversal_op(const F& f) const
    {
        traversal_op_act<sizeof...(Types) - 1,
                            std::tuple<Types...>&,
                            F&>::act((std::tuple<Types...>&)tpl_, (F&)f);
    }
    template<typename T>
    multievent operator, (const std::function<void(T&)>& f)
    {
        traversal_assign<sizeof...(Types) - 1>(f);
    }
    std::tuple<Types...> tpl_;
};
class multievent_begin
{
public:
    template<typename T>
    multievent<multievent_begin, T> operator , (const T& t)
    {
        return multievent<multievent_begin, T>(*this, t);
    }
};


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
typedef onevent<wxKeyEvent, 'kyup'>  Fonkeyup; Fonkeyup onkeyup;
typedef onevent<wxKeyEvent, 'kydn'>  Fonkeydown; Fonkeydown onkeydown;
typedef onevent<wxKeyEvent, 'char'>  Fonchar; Fonchar onchar;
typedef onevent<wxCommandEvent, 'clck'> Fonclick; Fonclick onclick;
typedef onevent<wxCommandEvent, 'text'> Fontext; Fontext ontext;
typedef onevent<wxDropFilesEvent, 'drpf'> Fondropfiles; Fondropfiles ondropfiles;
typedef onevent<wxCommandEvent, 'choc'> Fonchoice; Fonchoice onchoice;
typedef onevent<wxPaintEvent, 'pain'> Fonpaint; Fonpaint onpaint;
typedef onevent<wxScrollEvent, 'trck'> Fonthumbtrack; Fonthumbtrack onthumbtrack;
typedef onevent<wxScrollEvent, 'slup'> Fonscrollup; Fonscrollup onscrollup;
typedef onevent<wxScrollEvent, 'sldn'> Fonscrolldown; Fonscrolldown onscrolldown;
typedef onevent<wxScrollEvent, 'chng'> Fonscrollchanged; Fonscrollchanged onscrollchanged;
typedef onevent<wxListEvent, 'lact'> Fonlistactivated; Fonlistactivated onlistactivated;

#pragma GCC diagnostic pop


class layout_end {};
class menu_end {};
class layout_stretch_spacer {};

#define NO_IMPL(F) virtual void operator ()(F) {}
class prop_builder_itf
{
public:
    prop_builder_itf() :
        sf_(0), deferred_(false) {}
    virtual ~prop_builder_itf() {}
    virtual wxObject* get_prop() = 0;
    virtual void operator ()(wxSizerFlags) = 0;
    virtual void operator ()(wxSizer*) = 0;
    virtual void operator ()(long) = 0;
    //
    virtual void operator ()(const wxPoint&) = 0;
    virtual void operator ()(const wxSize&) = 0;
    // wxTextEntry
    virtual void operator ()(const wxArrayString&) = 0;
    virtual void operator ()(const wxString&) = 0;
    // wxWindoow
    virtual void operator ()(const Fonkeyup&) = 0;
    virtual void operator ()(const Fonkeydown&) = 0;
    virtual void operator ()(const Fonchar&) = 0;
    virtual void operator ()(const Fonclick&) = 0;
    virtual void operator ()(const Fontext&) = 0;
    virtual void operator ()(const Fondropfiles&) = 0;
    virtual void operator ()(const Fonchoice&) = 0;
    virtual void operator ()(const Fonpaint&) = 0;
    virtual void operator ()(const Fonthumbtrack&) = 0;
    virtual void operator ()(const Fonscrollup&) = 0;
    virtual void operator ()(const Fonscrolldown&) = 0;
    virtual void operator ()(const Fonscrollchanged&) = 0;
    virtual void operator ()(const Fonlistactivated&) = 0;
    // wxTextCtrl
    virtual void operator ()(const wxValidator&) = 0;

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
    NO_IMPL(const Fonchar&)
    NO_IMPL(const Fonclick&)
    NO_IMPL(const Fontext&)
    NO_IMPL(const Fondropfiles&)
    NO_IMPL(const Fonchoice&)
    NO_IMPL(const Fonpaint&)
    NO_IMPL(const wxArrayString&)
    NO_IMPL(const wxString&)
    NO_IMPL(const wxPoint&)
    NO_IMPL(const wxSize&)
    NO_IMPL(const Fonthumbtrack&)
    NO_IMPL(const Fonscrollup&)
    NO_IMPL(const Fonscrolldown&)
    NO_IMPL(const Fonscrollchanged&)
    NO_IMPL(const Fonlistactivated&)
    NO_IMPL(const wxValidator&)
};

template<class T>
class prop_builder_base : public no_impl_prop_builder_base
{
public:
    prop_builder_base(T* t) : t_(t) {}
    virtual wxObject* get_prop()
    {
        return t_;
    }
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
#define TEMPLATE_ACT_ONEVENT_BIND(E, F) \
    virtual void operator ()(const F& f) \
    {   \
        t_->Bind(E, f.functor_); \
    }
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_KEY_UP, Fonkeyup)
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_KEY_DOWN, Fonkeydown)
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_CHAR, Fonchar)
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_PAINT, Fonpaint)
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_SCROLL_THUMBTRACK, Fonthumbtrack)
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_SCROLL_LINEUP, Fonscrollup)
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_SCROLL_LINEDOWN, Fonscrolldown)
    TEMPLATE_ACT_ONEVENT_BIND(wxEVT_SCROLL_CHANGED, Fonscrollchanged)
#undef TEMPLATE_ACT_ONEVENT_BIND
    virtual void operator ()(const wxString& value)
    {
        wxTextEntry* te = NULL;
        wxWindow* win = NULL;
        if (te = dynamic_cast<wxTextEntry*>(t_))
            te->SetValue(value);
        else if (win = dynamic_cast<wxWindow*>(t_))
            win->SetLabel(value);
    }
    virtual void operator ()(const wxPoint& pos)
    {
        t_->SetPosition(pos);
    }
    virtual void operator ()(const wxSize& size)
    {
        t_->SetMinSize(size);
    }
    T* t_;
};

template<>
class prop_builder_base<wxSizer> : public no_impl_prop_builder_base
{
public:
    prop_builder_base(wxSizer* t) : t_(t) {}
    virtual wxObject* get_prop()
    {
        return t_;
    }
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
    virtual void operator ()(const Fontext& f)
    {
        t_->Bind(wxEVT_TEXT, f.functor_, t_->GetId());
    }
    virtual void operator ()(long style)
    {
        t_->SetWindowStyleFlag(style);
    }
    virtual void operator ()(const wxValidator& validator)
    {
        t_->SetValidator(validator);
    }
};

template<>
class prop_builder<wxChoice> : public prop_builder_base<wxChoice>
{
public:
    prop_builder(wxChoice* t) : prop_builder_base<wxChoice>(t) {}
    virtual void operator ()(const Fonchoice& f)
    {
        t_->Bind(wxEVT_CHOICE, f.functor_);
    }
    virtual void operator ()(const wxArrayString& items)
    {
        t_->Set(items);
    }
};

template<>
class prop_builder<wxListCtrl> : public prop_builder_base<wxListCtrl>
{
public:
    prop_builder(wxListCtrl* t) : prop_builder_base<wxListCtrl>(t) {}
    virtual void operator ()(const Fonlistactivated& f)
    {
        t_->Bind(wxEVT_LIST_ITEM_ACTIVATED, f.functor_);
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

    builder& operator () (layout_stretch_spacer&)
    {
        t_->AddStretchSpacer();
        return *this;
    }

    builder& operator [] (long style)
    {
        if (prop_)
            (*prop_)(style);
        return *this;
    }

    builder& operator [] (const wxArrayString& items)
    {
        if (prop_)
            (*prop_)(items);
        return *this;
    }

    template<typename Y>
    builder& operator [] (const std::function<void(Y*)>& onload)
    {
        wxObject* o = NULL;
        if (prop_)
            o = prop_->get_prop();
        Y* y = dynamic_cast<Y*>(o);
        if (!y)
            y = dynamic_cast<Y*>(t_);
        if (y)
            onload(y);
        return *this;
    }

#if __cplusplus < 201402L
	template<typename F>
	builder& operator [] (const lambda_transfer_deduce<F>& onload)
	{
		wxObject* o = NULL;
		if (prop_)
			o = prop_->get_prop();
		onload(o, t_);
		return *this;
	}
#endif 

    builder& operator [] (const Fonclick& f)
    {
        if (prop_)
            (*prop_)(f);
        return *this;
    }

    builder& operator [] (const Fondropfiles& f)
    {
        if (prop_)
            (*prop_)(f);
        return *this;
    }

    template<typename Functor>
    builder& operator [] (const Functor& f)
    {
        if (prop_)
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

    template<typename ... Types>
    builder& operator [] (const multievent<multievent_begin, Types...>& me)
    {
        if (prop_)
            me.traversal_op(*prop_);
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
    static lambda_transfer onload;
    static std::function<void(wxSizer*)> oncomplete;
    static layout_stretch_spacer stretch_spacer;
    template<class T>
    static builder<T>& begin(T* t) { return * new builder<T>(t);  }
};
layout_end layout::end;
lambda_transfer layout::onload;
std::function<void(wxSizer*)> layout::oncomplete;
layout_stretch_spacer layout::stretch_spacer;

#define EMPTY_ARGS wxEmptyString, wxDefaultPosition, wxDefaultSize

namespace listctlhlp
{

class cell;
class row;
class column;

class cell_builder;
class row_builder;
class column_builder;

class cell_end {};
class cell_another {};
class row_end {};
class row_another {};
class column_end {};

class cell_builder
{
public:
    explicit cell_builder(wxListCtrl* ctrl, bool modify = false)
        : ctrl_(ctrl), col_(0), row_(0), modify_(modify), inuse_(false)
    {
    }
    cell_builder& operator ()(int row, int col)
    {
        modify_if_needed();
        inuse_ = true;
        row_ = row;
        col_ = col;
        info_.Clear();
        info_.SetId(row_);
        info_.SetColumn(col_);
        if (ctrl_)
            ctrl_->GetItem(info_);
        return *this;
    }
    cell_builder& operator [](const wxString& txt)
    {
        info_.SetText(txt);
        return *this;
    }
    cell_builder& operator [](const char* txt)
    {
        info_.SetText(wxString(txt));
        return *this;
    }
    cell_builder& operator [](const wxColour& color)
    {
        info_.SetTextColour(color);
        return *this;
    }
    cell_builder& operator [](const wxFont& font)
    {
        info_.SetFont(font);
        return *this;
    }
    cell_builder& operator [](const int width)
    {
        info_.SetWidth(width);
        return *this;
    }
    wxListItem operator ()(cell_end&)
    {
        modify_if_needed();
        wxListItem ret = info_;
        delete this;
        return ret;
    }
    wxListItem operator ()(cell_another&)
    {
        modify_if_needed();
        wxListItem ret = info_;
        info_.Clear();
        return ret;
    }
private:
    void modify_if_needed()
    {
        if (!modify_ || !inuse_ || !ctrl_)
            return;

        ctrl_->SetItem(info_);
    }
    bool inuse_;
    bool modify_;
    int row_;
    int col_;
    wxListItem info_;
    wxListCtrl* ctrl_;
};

class cell
{
public:
    static cell_end end;
    static cell_another another;
    static cell_builder& begin(wxListCtrl* ctrl) { return *new cell_builder(ctrl, true); }
    static cell_builder& begin() { return *new cell_builder(NULL); }
};
cell_end cell::end;
cell_another cell::another;

class column_builder
{
public:
    column_builder(wxListCtrl* ctrl) : ctrl_(ctrl), col_(0) {}
    column_builder& operator ()(wxListItem* info)
    {
        finish_prev_cell();
        cell_.reset();
        ctrl_->InsertColumn(col_++, *info);
        return *this;
    }
    column_builder& operator ()(wxListItem info)
    {
        return operator ()(&info);
    }
    column_builder& operator ()(const wxString& txt)
    {
        finish_prev_cell();
        if (!cell_)
            cell_.reset(&cell::begin());
        (*cell_)[txt];
        //ctrl_->InsertColumn(col_++, txt);
        return *this;
    }
    column_builder& operator [](const wxColour& color)
    {
        if (cell_)
            (*cell_)[color];
        return *this;
    }
    column_builder& operator [](const wxFont& font)
    {
        if (cell_)
            (*cell_)[font];
        return *this;
    }
    column_builder& operator [](const int width)
    {
        if (cell_)
            (*cell_)[width];
        return *this;
    }
    wxListCtrl* operator ()(column_end&)
    {
        finish_prev_cell();
        wxListCtrl* ret = ctrl_;
        delete this;
        return ret;
    }
private:
    void finish_prev_cell();

    int col_;
    wxListCtrl* ctrl_;
    wxSharedPtr<cell_builder> cell_;
};
class column
{
public:
    static column_end end;
    static column_builder& begin(wxListCtrl* ctrl) { return *new column_builder(ctrl); }
};
column_end column::end;

class row_builder
{
public:
    row_builder(wxListCtrl* ctrl) : ctrl_(ctrl), col_(0)
    {
        row_ = ctrl->GetItemCount();
    }
    row_builder& operator ()(wxListItem* info)
    {
        finish_prev_cell();
        cell_.reset();

        info->SetColumn(col_++);
        info->SetId(row_);
        if (col_ == 0)
            row_ = ctrl_->InsertItem(*info);
        else
            ctrl_->SetItem(*info);
        return *this;
    }
    row_builder& operator ()(wxListItem info)
    {
        return operator ()(&info);
    }
    row_builder& operator ()(const wxString& txt)
    {
        finish_prev_cell();
        if (col_ == 0)
            row_ = ctrl_->InsertItem(row_, txt);
        //else
            //ctrl_->SetItem(row_, col_, txt);
        if (!cell_)
            cell_.reset(&cell::begin());

        cell_->operator ()(row_, col_++);
        (*cell_)[txt];
        //++col_;
        return *this;
    }
    row_builder& operator [](const wxColour& color)
    {
        if (cell_)
            (*cell_)[color];
        return *this;
    }
    row_builder& operator [](const wxFont& font)
    {
        if (cell_)
            (*cell_)[font];
        return *this;
    }
    row_builder& operator [](const int width)
    {
        if (cell_)
            (*cell_)[width];
        return *this;
    }
    wxListCtrl* operator ()(row_end&)
    {
        finish_prev_cell();
        wxListCtrl* ret = ctrl_;
        delete this;
        return ret;
    }
    row_builder& operator ()(row_another&)
    {
        finish_prev_cell();
        cell_.reset();
        row_++;
        col_ = 0;
        return *this;
    }
private:
    void finish_prev_cell();

    int row_;
    int col_;
    wxListCtrl* ctrl_;
    wxSharedPtr<cell_builder> cell_;
};
class row
{
public:
    static row_end end;
    static row_another another;
    static row_builder& begin(wxListCtrl* ctrl) { return *new row_builder(ctrl); }
};
row_end row::end;
row_another row::another;

void column_builder::finish_prev_cell()
{
    if (!cell_)
        return;

    ctrl_->InsertColumn(col_++, cell_->operator ()(cell::another));
}

void row_builder::finish_prev_cell()
{
    if (!cell_)
        return;
    wxListItem info = cell_->operator ()(cell::another);
    ctrl_->SetItem(info);
}


} // end namespace listctlhlp

struct widgets_enable_protector
{
    ~widgets_enable_protector()
    {
        release();
    };
    template<typename ... Ty>
    widgets_enable_protector(Ty... args)
    {
        this->operator() (args...);
    }
    widgets_enable_protector(const widgets_enable_protector& other)
    {
        widgets_ =  std::move(other.widgets_);
    }
    void operator ()(wxWindow* wid)
    {
        wid->Disable();
        widgets_.push_back(wid);
    }
    template<typename ... Ty>
    void operator ()(wxWindow* wid, Ty... args)
    {
        this->operator()(wid);
        this->operator()(args...);
    }
    void release()
    {
        std::for_each(std::begin(widgets_), std::end(widgets_),
                      [](wxWindow* w){ w->Enable(); });
    }
    std::vector<wxWindow*> widgets_;
};

} // end namespace wxWidgets

} // end namespace zhelper

#endif // end ZWX_HELPER__H_
