/*
 * Copyright (c) 2012-2026 Meltytech, LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENCODEDOCK_H
#define ENCODEDOCK_H

#include "settings.h"
#include "jobs/abstractjob.h"

#include <MltProperties.h>
#include <QDockWidget>
#include <QDomElement>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStringList>

class QTreeWidgetItem;
class QTemporaryFile;
namespace Ui {
class EncodeDock;
}

class MeltJob;
namespace Mlt {
class Service;
class Producer;
class Filter;
} // namespace Mlt

class PresetsProxyModel : public QSortFilterProxyModel
{
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

class EncodeDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit EncodeDock(QWidget *parent = 0);
    ~EncodeDock();

    void loadPresetFromProperties(Mlt::Properties &);
    bool isExportInProgress() const;

signals:
    void captureStateChanged(bool);

public slots:
    void on_exportButton_clicked();
    void on_resetButton_clicked();
    void on_actionReset_triggered();

private slots:
    void on_viewList_clicked(const QModelIndex &index);
    void on_videoCheckBox_toggled(bool checked);
    void on_audioCheckBox_toggled(bool checked);
    void on_useHardwareEncoderCheckBox_toggled(bool checked);
    void on_configureHardwareEncoderButton_clicked();
    void on_preset_selected(void *p);
    void on_preset_reset();
    void on_preset_deleted(const QString &name);
    void on_resX_editingFinished();
    void on_resY_editingFinished();
    void on_aspectX_editingFinished();
    void on_aspectY_editingFinished();
    void on_fps_editingFinished();
    void on_scanMode_currentIndexChanged(int index);
    void on_fieldOrder_currentIndexChanged(int index);
    void on_colorRange_currentIndexChanged(int index);
    void on_deinterlacer_currentIndexChanged(int index);
    void on_interpolation_currentIndexChanged(int index);
    void on_vcodec_currentIndexChanged(int index);
    void on_vrateControl_currentIndexChanged(int index);
    void on_vbitrate_editingFinished();
    void on_vquality_valueChanged(int value);
    void on_gop_editingFinished();
    void on_bframes_editingFinished();
    void on_acodec_currentIndexChanged(int index);
    void on_arateControl_currentIndexChanged(int index);
    void on_abitrate_editingFinished();
    void on_aquality_valueChanged(int value);
    void on_samplerate_editingFinished();
    void on_format_currentIndexChanged(int index);
    void on_parallel_toggled(bool checked);
    void on_openOtherTriggered();
    void on_tabWidget_currentChanged(int index);

private:
    Ui::EncodeDock *ui;
    QStandardItemModel *m_presetsModel;
    PresetsProxyModel *m_presetsProxyModel;
    QString m_currentPreset;
    QString m_customPresetPath;
    bool m_isUpdating;
    Mlt::Properties m_properties;
    Mlt::Properties m_defaultProperties;

    void setupPresets();
    void loadPresets();
    void savePreset(const QString &name);
    void deletePreset(const QString &name);
    void updateUiFromProperties();
    void updatePropertiesFromUi();
    void setDefaults();
    void showHardwareEncoderError(const QString &message);
    void checkHardwareEncoder();
    void updateResampleWarning();
    void setReframeEnabled(bool enabled);
    void showResampleWarning(const QString &message);
    void hideResampleWarning(bool hide = true);
    void checkFrameRate();
    void setResolutionAspectFromProfile();
    void collectProperties(QDomElement &node, int realtime);
    void setSubtitleProperties(QDomElement &node, Mlt::Producer *service);
    MeltJob *createJob(
        Mlt::Producer *service, QDomDocument &dom, const QString &target, int realtime, int pass);
};

#endif // ENCODEDOCK_H
