#include "ribbon.h"
#include <QHBoxLayout>
#include <base/lang/scope.hpp>
#include <base/lang/move.hpp>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QApplication>
#include <QFrame>
#include <QLineEdit>
#include <QTabBar>
#include <QWidgetAction>
#include <base/io/file/file.hpp>
#include <variant>
#include <QButtonGroup>
#include <QDebug>

using namespace std;

Ribbon::Ribbon(QWidget *parent)
    :QTabWidget (parent)
{
    this->setTabBar (new RibbonBar (this));

    setMaximumHeight (130);
    setMinimumHeight (130);

    setup_ui ();
}

void Ribbon::setupTab(gsl::span<UiGroup> group, const QString &name)
{
    addTab (makeTab (group).release (), name);
}

std::unique_ptr<RibbonTool> Ribbon::makeButton(const QPixmap &raw_icon, const QString &text)
{
    const auto icon = raw_icon.scaled (buttonSize);
    auto button = make_unique<RibbonTool> ();

    button->setIconSize (icon.size ());
    button->setIcon (icon);
    button->setText (text);
    button->setObjectName (text);
    button->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
    button->setAutoRaise (true);

    return button;
}

std::unique_ptr<RibbonTool> Ribbon::makeIcon(const QPixmap &icon, const QString &text)
{
    auto button = make_unique<RibbonTool> ();

    button->setIconSize (iconSize);
    button->setIcon (icon);
    button->setText (text);
    button->setObjectName (text);
    button->setToolTip (text);

    button->setCheckable (true);

    return button;
}


void Ribbon::setup_ui()
{
    setup_menu ();

}


unique_ptr<QWidgetAction> make_action (const QPixmap & pix, const QString & text, QWidget * parent = nullptr)
{
    auto action = make_unique<QWidgetAction> (parent);
    action->setDefaultWidget (new RibbonMenuItem(pix, text, parent));

    return action;
}

void Ribbon::setup_menu()
{
    auto file = new RibbonButton ("  文件", this);
    auto menu = new RibbonMenu (file);
    file->setMenu (menu);

    auto action =  make_action (QPixmap ("png/新建.png"), "新建");
    connect (action.get (), &QAction::triggered, this, &Ribbon::fileNew);
    menu->addAction (action.release ());

    action =  make_action (QPixmap ("png/打开.png"), "打开");
    connect (action.get (), &QAction::triggered, this, &Ribbon::fileOpen);
    menu->addAction (action.release ());

    action =  make_action (QPixmap ("png/保存.png"), "保存");
    connect (action.get (), &QAction::triggered, this, &Ribbon::fileSave);
    connect (this, &Ribbon::subwindowActive, action.get(), &QAction::setEnabled);
    menu->addAction (action.release ());

    action =  make_action (QPixmap ("png/另存为.png"), "另存为");
    connect (action.get (), &QAction::triggered, this, &Ribbon::fileSaveAs);
    connect (this, &Ribbon::subwindowActive, action.get(), &QAction::setEnabled);
    menu->addAction (action.release ());

    action =  make_action (QPixmap ("png/退出.png"), "退出");
    connect (action.get (), &QAction::triggered, this, &Ribbon::fileExit);
    menu->addAction (action.release ());

    menu->setContentsMargins(10, 0, 0, 0);
}

std::unique_ptr<QWidget> Ribbon::makeTab(gsl::span<UiGroup> ui)
{
    auto ret = make_unique<QWidget> ();

    auto layout = make_unique<QHBoxLayout> ();

    layout->setContentsMargins (1, 1, 1, 1);
    layout->setSpacing (1);
    for (auto & it : ui)
    {
        if (it.index () == 0)
        {
            auto g = std::move (get<0> (it));
            setupButtonBlock (ret.get (), layout.get (), std::move (g));
        }
        else if (it.index () == 1)
        {
            auto g = std::move (get<1> (it));
            setupIconBlock (ret.get (), layout.get (), std::move (g));
        }
    }

    layout->addItem (new QSpacerItem (0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    ret->setLayout (layout.release ());
    return ret;
}

void Ribbon::setupButtonBlock (QWidget * ret, QBoxLayout * layout, ButtonCell g)
{
    auto block_layout = make_unique<QVBoxLayout> ();
    block_layout->setContentsMargins (1, 1, 1, 1);
    block_layout->setSpacing (1);

    auto upper_layout = make_unique<QHBoxLayout> ();
    upper_layout->setSpacing(10);
    upper_layout->setContentsMargins(10, 0, 10, 0);

    for (auto & it : g)
    {
        auto btn = std::move (it.first);
        if (it.second != nullptr)
        {
            (it.second) (btn.get ());
        }

        upper_layout->addWidget (btn.release ());
    }

    block_layout->addLayout (upper_layout.release ());
    auto label = new QLabel (g.title ());
    label->setAlignment (Qt::AlignHCenter | Qt::AlignBottom);
    block_layout->addWidget (label);
    layout->addLayout (block_layout.release (), 0);

    auto line = new QFrame (ret);
    line->setFrameShape (QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget (line);

}

void Ribbon::setupIconBlock(QWidget * ret, QBoxLayout * layout, FrameGroup g)
{
    auto block_layout = make_unique<QVBoxLayout> ();
    block_layout->setContentsMargins (1, 1, 1, 1);
    block_layout->setSpacing (1);

    auto frame = new QFrame (ret);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    auto frame_layout = make_unique<QHBoxLayout> ();
    frame_layout->setContentsMargins(1, 1, 1, 1);
    frame_layout->setSpacing(1);

    for (auto & it : g)
    {
        auto btn = ::move (it.first);
        if (it.second != nullptr)
        {
            (it.second) (btn.get ());
        }
        frame_layout->addWidget (btn.release ());
    }
    frame->setLayout (frame_layout.release ());
    block_layout->addWidget (frame);


    auto label = new QLabel (g.title ());
    label->setAlignment (Qt::AlignHCenter | Qt::AlignBottom);
    block_layout->addWidget (label);
    layout->addLayout (block_layout.release ());

    auto line = new QFrame (ret);
    line->setFrameShape (QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget (line);
}
























