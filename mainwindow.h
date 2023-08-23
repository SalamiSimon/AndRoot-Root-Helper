#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int dotCount = 1;

private slots:
    void on_detectADBDevice_clicked();

    void on_bootFastboot_clicked();

    void on_exitBootloader_clicked();

    void on_detectFastboot_clicked();

    void on_binBtn_clicked();

    void on_zipBtn_clicked();

    void on_filePathBtn_clicked();

    void on_extractPayload_clicked();

    void onExtractionCompleted();

    void disableAllBtn(bool disable);

    void on_openImagePath_clicked();

    void on_imgBtn_clicked();

    void extractFileFromZip(const QString& zipFilePath, const QString& filePathInZip, const QString& outputPath);

    void on_USBDebugging_stateChanged();

    void on_backedUp_stateChanged();

    void on_bootloaderUnlocked_stateChanged();

    void on_info_clicked();

    void on_pushIMG_clicked();

    void on_pullIMG_clicked();

private:
    Ui::MainWindow *ui;

    QTimer* loadingTimer;
    QString imagePath;

    bool inFastboot = false;

    QString adbPath = "platform-tools/adb.exe";
    QString cmdPath = "platform-tools/cmd.exe";

    QPoint  dragStartPosition;
};
#endif // MAINWINDOW_H
