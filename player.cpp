/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "player.h"
#include "image.h"

#include "playercontrols.h"
#include "playlistmodel.h"
#include "videowidget.h"

#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>

#include <QTextStream>

Player::Player(QWidget *parent)
    : QWidget(parent)
{
//! [create-objs]
    m_playerPrimary = new QMediaPlayer(this);
    m_playerPrimary->setAudioRole(QAudio::VideoRole);
    qInfo() << "Supported audio roles:";
    for (QAudio::Role role : m_playerPrimary->supportedAudioRoles())
        qInfo() << "    " << role;
    // owned by PlaylistModel
    m_playlist_primary = new QMediaPlaylist();
    m_playerPrimary->setPlaylist(m_playlist_primary);

    m_playerSecondary = new QMediaPlayer(this);
    m_playerSecondary->setAudioRole(QAudio::VideoRole);
    m_playlist_secondary = new QMediaPlaylist();
    m_playerSecondary->setPlaylist(m_playlist_secondary);

//! [create-objs]

    connect(m_playerPrimary, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(m_playerPrimary, &QMediaPlayer::positionChanged, this, &Player::positionChanged);
    connect(m_playerPrimary, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &Player::metaDataChanged);
    connect(m_playlist_primary, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged);
    connect(m_playerPrimary, &QMediaPlayer::mediaStatusChanged, this, &Player::statusChanged);
    connect(m_playerPrimary, &QMediaPlayer::bufferStatusChanged, this, &Player::bufferingProgress);
    connect(m_playerPrimary, &QMediaPlayer::videoAvailableChanged, this, &Player::videoAvailableChanged);
    connect(m_playerPrimary, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);
    connect(m_playerPrimary, &QMediaPlayer::stateChanged, this, &Player::stateChanged);

    connect(m_playerSecondary, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(m_playerSecondary, &QMediaPlayer::positionChanged, this, &Player::positionChanged);
    connect(m_playerSecondary, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &Player::metaDataChanged);
    connect(m_playlist_secondary, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged);
    connect(m_playerSecondary, &QMediaPlayer::mediaStatusChanged, this, &Player::statusChanged);
    connect(m_playerSecondary, &QMediaPlayer::bufferStatusChanged, this, &Player::bufferingProgress);
    connect(m_playerSecondary, &QMediaPlayer::videoAvailableChanged, this, &Player::videoAvailableChanged);
    connect(m_playerSecondary, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);
    connect(m_playerSecondary, &QMediaPlayer::stateChanged, this, &Player::stateChanged);

//! [2]
    //Primary Video Widget
    primaryVideoWidget = new VideoWidget(this);
    m_playerPrimary->setVideoOutput(primaryVideoWidget);

    m_playlistModel = new PlaylistModel(this);
    m_playlistModel->setPlaylist(m_playlist_primary);

    //Secondary Video Widget
    secondaryVideoWidget = new VideoWidget(this);
    //m_player->setVideoOutput(secondaryVideoWidget);

//! [2]
    m_playlistView = new QListView(this);
    m_playlistView->setModel(m_playlistModel);
    m_playlistView->setCurrentIndex(m_playlistModel->index(m_playlist_primary->currentIndex(), 0));

    connect(m_playlistView, &QAbstractItemView::activated, this, &Player::jump);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0, m_playerPrimary->duration() / 1000);

    m_labelDuration = new QLabel(this);
    connect(m_slider, &QSlider::sliderMoved, this, &Player::seek);

    QPushButton *openButton = new QPushButton(tr("Open"), this);

    connect(openButton, &QPushButton::clicked, this, &Player::load);

    PlayerControls *controls = new PlayerControls(this);
    controls->setState(m_playerPrimary->state());
    controls->setVolume(m_playerPrimary->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, &PlayerControls::play, m_playerPrimary, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, m_playerPrimary, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, m_playerPrimary, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::next, m_playlist_primary, &QMediaPlaylist::next);
    connect(controls, &PlayerControls::previous, this, &Player::previousClicked);
    connect(controls, &PlayerControls::changeVolume, m_playerPrimary, &QMediaPlayer::setVolume);
    connect(controls, &PlayerControls::changeMuting, m_playerPrimary, &QMediaPlayer::setMuted);
    connect(controls, &PlayerControls::changeRate, m_playerPrimary, &QMediaPlayer::setPlaybackRate);
    connect(controls, &PlayerControls::stop, primaryVideoWidget, QOverload<>::of(&QVideoWidget::update));

    connect(m_playerPrimary, &QMediaPlayer::stateChanged, controls, &PlayerControls::setState);
    connect(m_playerPrimary, &QMediaPlayer::volumeChanged, controls, &PlayerControls::setVolume);
    connect(m_playerPrimary, &QMediaPlayer::mutedChanged, controls, &PlayerControls::setMuted);

    //Buttons
    connectVideoButton = new QPushButton("Connect Video", this);
    saveButton = new QPushButton("Save", this);

    m_fullScreenButton = new QPushButton(tr("FullScreen"), this);
    m_fullScreenButton->setCheckable(true);

    m_colorButton = new QPushButton(tr("Color Options..."), this);
    m_colorButton->setEnabled(false);

    connect(connectVideoButton, &QPushButton::clicked, this, &Player::connectVideo);
    connect(saveButton, &QPushButton::clicked, this, &Player::saveFile);
    connect(m_colorButton, &QPushButton::clicked, this, &Player::showColorDialog);

    /****************************************************************/
    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(primaryVideoWidget,2);
    displayLayout->addWidget(m_playlistView,1);
    displayLayout->addWidget(secondaryVideoWidget,2);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);
    controlLayout->addWidget(m_fullScreenButton);
    controlLayout->addWidget(m_colorButton);
    controlLayout->addWidget(connectVideoButton);
    controlLayout->addWidget(saveButton);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_slider);
    hLayout->addWidget(m_labelDuration);
    layout->addLayout(hLayout);
    layout->addLayout(controlLayout);
#if defined(Q_OS_QNX)
    // On QNX, the main window doesn't have a title bar (or any other decorations).
    // Create a status bar for the status information instead.
    m_statusLabel = new QLabel;
    m_statusBar = new QStatusBar;
    m_statusBar->addPermanentWidget(m_statusLabel);
    m_statusBar->setSizeGripEnabled(false); // Without mouse grabbing, it doesn't work very well.
    layout->addWidget(m_statusBar);
#endif

    setLayout(layout);

    if (!isPlayerAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));

        controls->setEnabled(false);
        m_playlistView->setEnabled(false);
        openButton->setEnabled(false);
        m_colorButton->setEnabled(false);
        m_fullScreenButton->setEnabled(false);
    }

    metaDataChanged();
}

Player::~Player()
{
}

bool Player::isPlayerAvailable() const
{
    return m_playerPrimary->isAvailable();
}

void Player::load()
{
    /*Upload Files Here*/
    //QImage for RGB Images
    //QVideoFrame

    QFileDialog uploadDialog(this);
    uploadDialog.setAcceptMode(QFileDialog::AcceptOpen);
    uploadDialog.setWindowTitle("Import Files");

    QTextStream out(stdout);
    QStringList fileNames = uploadDialog.getOpenFileNames(this,"Select Files to Upload", QDir::homePath(), tr("(*rgb);;AllFiles(*)"));
    if(fileNames.count() > 0)
    {
        for(int fn = 0; fn < fileNames.count(); fn++)
        {
            const char* imagePath = fileNames.at(fn).toLocal8Bit().data();

            Image image;
            image.setHeight(352);
            image.setWidth(288);
            image.setImagePath(imagePath);
            image.ReadImage();

            Images.push_back(new QImage((const unsigned char*)image.getImageData(), 352, 288, QImage::Format_RGB32));

        }
    }
}

void Player::connectVideo()
{

}

void Player::saveFile()
{

}

static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void Player::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            m_playlist_primary->load(url);
        else
            m_playlist_primary->addMedia(url);
    }
}

void Player::setCustomAudioRole(const QString &role)
{
    m_playerPrimary->setCustomAudioRole(role);
}

void Player::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_slider->setMaximum(m_duration);
}

void Player::positionChanged(qint64 progress)
{
    if (!m_slider->isSliderDown())
        m_slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void Player::metaDataChanged()
{
    if (m_playerPrimary->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(m_playerPrimary->metaData(QMediaMetaData::AlbumArtist).toString())
                .arg(m_playerPrimary->metaData(QMediaMetaData::Title).toString()));

        if (m_coverLabel) {
            QUrl url = m_playerPrimary->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            m_coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
}

void Player::previousClicked()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (m_playerPrimary->position() <= 5000)
        m_playlist_primary->previous();
    else
        m_playerPrimary->setPosition(0);
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist_primary->setCurrentIndex(index.row());
        m_playerPrimary->play();
    }
}

void Player::playlistPositionChanged(int currentItem)
{
    m_playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
}

void Player::seek(int seconds)
{
    m_playerPrimary->setPosition(seconds * 1000);
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(tr("Buffering %1%").arg(m_playerPrimary->bufferStatus()));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Stalled %1%").arg(m_playerPrimary->bufferStatus()));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void Player::stateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState)
    {
        //clearHistogram();
    }
}

void Player::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
        status == QMediaPlayer::BufferingMedia ||
        status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void Player::bufferingProgress(int progress)
{
    if (m_playerPrimary->mediaStatus() == QMediaPlayer::StalledMedia)
        setStatusInfo(tr("Stalled %1%").arg(progress));
    else
        setStatusInfo(tr("Buffering %1%").arg(progress));
}

void Player::videoAvailableChanged(bool available)
{
    if (!available) {
        disconnect(m_fullScreenButton, &QPushButton::clicked, primaryVideoWidget, &QVideoWidget::setFullScreen);
        disconnect(primaryVideoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);
        primaryVideoWidget->setFullScreen(false);
    } else {
        connect(m_fullScreenButton, &QPushButton::clicked, primaryVideoWidget, &QVideoWidget::setFullScreen);
        connect(primaryVideoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);

        if (m_fullScreenButton->isChecked())
            primaryVideoWidget->setFullScreen(true);
    }
    m_colorButton->setEnabled(available);
}

void Player::setTrackInfo(const QString &info)
{
    m_trackInfo = info;

    if (m_statusBar) {
        m_statusBar->showMessage(m_trackInfo);
        m_statusLabel->setText(m_statusInfo);
    } else {
        if (!m_statusInfo.isEmpty())
            setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
        else
            setWindowTitle(m_trackInfo);
    }
}

void Player::setStatusInfo(const QString &info)
{
    m_statusInfo = info;

    if (m_statusBar) {
        m_statusBar->showMessage(m_trackInfo);
        m_statusLabel->setText(m_statusInfo);
    } else {
        if (!m_statusInfo.isEmpty())
            setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
        else
            setWindowTitle(m_trackInfo);
    }
}

void Player::displayErrorMessage()
{
    setStatusInfo(m_playerPrimary->errorString());
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}

void Player::showColorDialog()
{
    if (!m_colorDialog) {
        QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
        brightnessSlider->setRange(-100, 100);
        brightnessSlider->setValue(primaryVideoWidget->brightness());
        connect(brightnessSlider, &QSlider::sliderMoved, primaryVideoWidget, &QVideoWidget::setBrightness);
        connect(primaryVideoWidget, &QVideoWidget::brightnessChanged, brightnessSlider, &QSlider::setValue);

        QSlider *contrastSlider = new QSlider(Qt::Horizontal);
        contrastSlider->setRange(-100, 100);
        contrastSlider->setValue(primaryVideoWidget->contrast());
        connect(contrastSlider, &QSlider::sliderMoved, primaryVideoWidget, &QVideoWidget::setContrast);
        connect(primaryVideoWidget, &QVideoWidget::contrastChanged, contrastSlider, &QSlider::setValue);

        QSlider *hueSlider = new QSlider(Qt::Horizontal);
        hueSlider->setRange(-100, 100);
        hueSlider->setValue(primaryVideoWidget->hue());
        connect(hueSlider, &QSlider::sliderMoved, primaryVideoWidget, &QVideoWidget::setHue);
        connect(primaryVideoWidget, &QVideoWidget::hueChanged, hueSlider, &QSlider::setValue);

        QSlider *saturationSlider = new QSlider(Qt::Horizontal);
        saturationSlider->setRange(-100, 100);
        saturationSlider->setValue(primaryVideoWidget->saturation());
        connect(saturationSlider, &QSlider::sliderMoved, primaryVideoWidget, &QVideoWidget::setSaturation);
        connect(primaryVideoWidget, &QVideoWidget::saturationChanged, saturationSlider, &QSlider::setValue);

        QFormLayout *layout = new QFormLayout;
        layout->addRow(tr("Brightness"), brightnessSlider);
        layout->addRow(tr("Contrast"), contrastSlider);
        layout->addRow(tr("Hue"), hueSlider);
        layout->addRow(tr("Saturation"), saturationSlider);

        QPushButton *button = new QPushButton(tr("Close"));
        layout->addRow(button);

        m_colorDialog = new QDialog(this);
        m_colorDialog->setWindowTitle(tr("Color Options"));
        m_colorDialog->setLayout(layout);

        connect(button, &QPushButton::clicked, m_colorDialog, &QDialog::close);
    }
    m_colorDialog->show();
}
