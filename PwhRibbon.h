#pragma once
#include "Ribbon.h"

class PwhRibbon : public Ribbon
{
    Q_OBJECT
public:
    PwhRibbon(QWidget* parent = nullptr);
    void initConn();
};

