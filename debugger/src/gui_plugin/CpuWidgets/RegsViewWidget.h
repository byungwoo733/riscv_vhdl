/*
 *  Copyright 2018 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include "api_core.h"   // MUST BE BEFORE QtWidgets.h or any other Qt header.
#include "attribute.h"
#include "igui.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QAction>
#include <QtCore/QEvent>
#include <QtGui/qevent.h>

namespace debugger {

class RegsViewWidget : public QWidget,
                       public IGuiCmdHandler {
    Q_OBJECT
 public:
    RegsViewWidget(IGui *igui, QWidget *parent = 0);
    virtual ~RegsViewWidget();

    /** IGuiCmdHandler */
    virtual void handleResponse(AttributeType *req, AttributeType *resp);

 signals:
    void signalHandleResponse(AttributeType *resp);

 private slots:
    void slotUpdateByTimer();
    void slotRegChanged(AttributeType *wrcmd);

 private:
    void addRegWidget(int row, int col, int bytes, const char *name);

 private:
    AttributeType cmdRegs_;
    AttributeType listRegs_;
    AttributeType resp_;
    QGridLayout *gridLayout;
    
    IGui *igui_;
    bool waitingResp_;
};

class RegsQMdiSubWindow : public QMdiSubWindow {
    Q_OBJECT
public:
    RegsQMdiSubWindow(IGui *igui, QMdiArea *area, QWidget *parent,
                      QAction *act = 0)
        : QMdiSubWindow(parent) {
        setAttribute(Qt::WA_DeleteOnClose);
        action_ = act;
        area_ = area;

        setWindowTitle(tr("Registers"));
        setWindowIcon(QIcon(tr(":/images/cpu_96x96.png")));
        QWidget *pnew = new RegsViewWidget(igui, this);
        if (act) {
            act->setChecked(true);
        }
        connect(parent, SIGNAL(signalUpdateByTimer()),
                pnew, SLOT(slotUpdateByTimer()));

        setWidget(pnew);
        area_->addSubWindow(this);
        show();
    }
    
 protected:
    void closeEvent(QCloseEvent *event_) Q_DECL_OVERRIDE {
        if (action_) {
            action_->setChecked(false);
        }
        area_->removeSubWindow(this);
        event_->accept();
    }
 private:
    QAction *action_;
    QMdiArea *area_;
};

}  // namespace debugger
