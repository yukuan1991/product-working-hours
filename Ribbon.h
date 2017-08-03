#pragma once
#include <QTabWidget>
#include <base/lang/not_null.h>
#include <memory>
#include <QToolButton>
#include <QPushButton>
#include <QTabBar>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyleOption>
#include <QLineEdit>
#include <QPainter>
#include <QMenu>
#include <gsl/span>
#include <variant>
#include <functional>

class RibbonTool;
class RibbonButton;

class Ribbon : public QTabWidget
{
    Q_OBJECT
signals:
    void fileNew ();
    void fileOpen ();
    void fileSave ();
    void fileSaveAs ();
    void fileExit ();
    void subwindowActive (bool);
protected:
    using ButtonInit = std::function<void (RibbonTool*)>;

    struct ButtonCell
    {
        void add (QString text, const QPixmap & pix, RibbonTool *& member)
        {
            auto init_code = [member = &member] (RibbonTool * tool) { *member = tool; };
            auto tool = makeButton (pix, text);
            data_.emplace_back (std::move (tool), init_code);
        }
        void add (QString text, const QPixmap & pix, ButtonInit init_code = nullptr)
        {
            auto tool = makeButton (pix, text);
            data_.emplace_back (std::move (tool), init_code);
        }
        void add (std::unique_ptr<RibbonTool> tool, ButtonInit init_code = nullptr)
        {
            data_.emplace_back (std::move (tool), init_code);
        }
        QString title () const noexcept { return title_; }
        void set_title (const QString & title) noexcept { title_ = title; }
        auto begin () const noexcept { return data_.begin (); }
        auto begin () noexcept { return data_.begin (); }
        auto end () const noexcept { return data_.end (); }
        auto end () noexcept { return data_.end (); }
    private:
        std::vector<std::pair<std::unique_ptr<RibbonTool>, ButtonInit>> data_;
        QString title_;
    };

    struct FrameGroup
    {
        void add (QString text, const QPixmap & pix, RibbonTool *& member)
        {
            auto init_code = [member = &member] (RibbonTool * tool) { *member = tool; };
            add (text, pix, init_code);
        }
        void add (QString text, const QPixmap & pix, ButtonInit init_code = nullptr)
        {
            auto tool = makeIcon (pix, text);
            data_.emplace_back (std::move (tool), init_code);
        }
        QString title () const noexcept { return title_; }
        void setTitle (const QString & title) noexcept { title_ = title; }

        auto begin () const noexcept { return data_.begin (); }
        auto begin () noexcept { return data_.begin (); }
        auto end () const noexcept { return data_.end (); }
        auto end () noexcept { return data_.end (); }

    private:
        QString title_;
        std::vector<std::pair<std::unique_ptr<RibbonTool>, ButtonInit>> data_;
    };
    using UiGroup = std::variant<ButtonCell, FrameGroup>;
public:
    void setupTab(gsl::span<UiGroup> group, const QString &name);
    Ribbon (QWidget * parent = nullptr);
protected:
    static std::unique_ptr<RibbonTool> makeButton (const QPixmap & icon, const QString & text);
    static std::unique_ptr<RibbonTool> makeIcon (const QPixmap & icon, const QString & text);

    void setup_ui ();
    void setup_menu ();
    std::unique_ptr<QLayout> setupLayout ();
    std::unique_ptr<QWidget> makeTab (gsl::span<UiGroup> ui);
    static void setupButtonBlock (QWidget * ret, QBoxLayout *layout, ButtonCell g);
    static void setupIconBlock (QWidget * ret, QBoxLayout * layout, FrameGroup g);

    static constexpr auto buttonSize = QSize (40, 40);
    static constexpr auto iconSize = QSize (32, 32);
};

class RibbonButton : public QPushButton
{
    Q_OBJECT
public:
    template<typename ... Args>
    RibbonButton (Args && ... p) : QPushButton (std::forward<Args> (p)...) {}
};

class RibbonMenu : public QMenu
{
    Q_OBJECT
public:
    template<typename ... Args>
    RibbonMenu (Args && ... p) : QMenu (std::forward<Args> (p)...) {}
};

class RibbonTool : public QToolButton
{
    Q_OBJECT
public:
    template<typename ... Args>
    RibbonTool (Args && ... p) : QToolButton (std::forward<Args> (p)...) {}
};

class RibbonBar : public QTabBar
{
    Q_OBJECT
public:
    RibbonBar (QWidget * parent = nullptr) : QTabBar (parent) {}
};

class RibbonEdit : public QLineEdit
{
    Q_OBJECT
public:
    template<typename ... Args>
    RibbonEdit (Args && ... p) : QLineEdit (std::forward<Args> (p)...) {}
};

class LayoutHorizontal : public QHBoxLayout
{
    Q_OBJECT
public:
    template<typename ... Args>
    LayoutHorizontal (Args && ... p) : QHBoxLayout (std::forward<Args> (p)...) {}
    void set_w (int w) { w_ = w; }
protected:
    QSize maximumSize () const override
    {
        if (w_ == -1)
        {
            return QHBoxLayout::maximumSize ();
        }
        else
        {
            return QSize (w_, QHBoxLayout::maximumSize ().height ());
        }
    }
    QSize minimumSize () const override
    {
        if (w_ == -1)
        {
            return QHBoxLayout::minimumSize ();
        }
        else
        {
            return QSize (w_, QHBoxLayout::minimumSize ().height ());
        }
    }
private:
    int w_ = -1;
};



class RibbonMenuItem : public QWidget
{
    Q_OBJECT
public:
    RibbonMenuItem(const QPixmap& pix, const QString& text, QWidget *parent = 0)
        :QWidget (parent)
    {
        QLabel* labelIcon = new QLabel(this);
        labelIcon->setFixedSize(24, 24);
        labelIcon->setScaledContents(true);
        labelIcon->setPixmap(pix);

        QLabel* labelText = new QLabel(text,this);
        labelText->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);

        QHBoxLayout* layout = new QHBoxLayout;
        layout->setContentsMargins(1, 1, 1, 1);
        layout->setSpacing(1);
        layout->addWidget(labelIcon);
        layout->addWidget(labelText);
        setLayout(layout);

        setFixedWidth(100);
        setFixedHeight (32);
    }
    ~RibbonMenuItem()
    {}

protected:
    void paintEvent(QPaintEvent*) override {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

private:

};




