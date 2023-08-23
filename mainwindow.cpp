#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QDebug>
#include <QFileDialog>

#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QString>

#include <QtConcurrent>
#include <QFutureWatcher>
#include <QTimer>

#include <QDesktopServices>
#include <QMessageBox>
#include <QMouseEvent>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    loadingTimer = new QTimer(this);
    ui->setupUi(this);
    disableAllBtn(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void showCustomAlert(const QString& title, const QString& text)
{
    // Create a custom QMessageBox
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(QMessageBox::Information);

    // Add an "OK" button
    msgBox.addButton("OK", QMessageBox::AcceptRole);

    msgBox.setStyleSheet("QMessageBox {"
                         "background-color: white;" // Background color
                         "border: 2px solid black;"   // Border style
                         "border-radius: 5px;"
                         "}"
                         "QLabel {"
                         "color: #333;"              // Text color
                         "font-size: 16px;"          // Font size
                         "}"
                         "QPushButton {"
                         "background-color: rgba(250, 0, 99, 0.85);" // Button background color
                         "color: black;"               // Button text color
                         "border: 2px solid black;"    // Remove button borders
                         "border-radius: 15px;"
                         "padding: 5px 10px;"         // Button padding
                         "}"
                         "QPushButton:hover {"
                         "background-color: #005eaa;" // Hover effect
                         "}");

    // Execute the QMessageBox and handle button clicks (only "OK" button)
    msgBox.exec();
}

void MainWindow::on_detectADBDevice_clicked()
{
    // Check if the file exists
    QFile adbFile(adbPath);
    QFile cmdFile(cmdPath);

    if (!adbFile.exists())
    {
        showCustomAlert("Error", "ADB executable not found!");
        ui->deviceStatus->setStyleSheet("color: red;");
        return;
    }

    if (!cmdFile.exists())
    {
        showCustomAlert("Error", "CMD executable not found!");
        ui->deviceStatus->setStyleSheet("color: red;");
        return;
    }

    QProcess process;
    process.setWorkingDirectory("platform-tools");
    process.start("cmd.exe", QStringList() << "/c" << "adb devices");
    process.waitForFinished();

    QString output = process.readAllStandardOutput();

    // Extract device ID

    QString startText = "List of devices attached\r\n";
    QString endText = "\tdevice\r\n\r\n";
    int startIndex = output.indexOf(startText) + startText.length();
    int endIndex = output.indexOf(endText);
    QString deviceID = output.mid(startIndex, endIndex - startIndex);


    if(deviceID != "\r\n")
    {
        ui->deviceStatus->setText("Device Connected!");
        ui->deviceStatus->setStyleSheet("color: green;");
        ui->deviceIDLabel->setStyleSheet("color: green;");
        ui->deviceIDLabel->setText("Device ID: " + deviceID);
    }
    else
    {
        showCustomAlert("Error", "Device not detected.\nMake sure it's properly connected and try again.");
        ui->deviceStatus->setStyleSheet("color: red;");
    }

    process.setWorkingDirectory("platform-tools");
    process.start("cmd.exe", QStringList() << "/c" << "adb shell getprop ro.bootmode");
    process.waitForFinished();
    QString bootState = process.readAllStandardOutput();

    if (bootState.contains("unauthorized", Qt::CaseInsensitive)){
        ui->deviceStatus->setText("Device is not authorized");
        ui->deviceStatus->setStyleSheet("color: red;");
    }

}

void MainWindow::on_bootFastboot_clicked()
{
    QProcess process;
    process.setWorkingDirectory("platform-tools");
    process.start("cmd.exe", QStringList() << "/c" << "adb reboot bootloader");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();
    qDebug() << error;
    qDebug() << output;

    if(!error.contains("no devices/emulators found"))
    {
        showCustomAlert("Success", "Device sucessfully booted into Bootloader!");
        ui->bootloaderLabel->setStyleSheet("color: green;");
        ui->exitBootloader->setEnabled(true);
        ui->bootState->setStyleSheet("color: green;");
        ui->bootState->setText("Boot State: Fastboot");
        inFastboot = true;
    }
    else
    {
        QMessageBox::information(this, "error", error);
        showCustomAlert("Error", "Could't boot into Bootloader.\nMake sure your device is properly connected and try again.");
        ui->bootloaderLabel->setStyleSheet("color: red;");
        ui->bootState->setText("Boot State: Normal Boot");
    }
}

void MainWindow::on_exitBootloader_clicked()
{
    QProcess process;
    process.setWorkingDirectory("platform-tools");
    process.start("cmd.exe", QStringList() << "/c" << "fastboot devices");

    // Wait for the process to start
    if (process.waitForStarted()) {
        // Wait for the process to finish or timeout after 10 seconds
        if (process.waitForFinished(2000)) {
            ui->bootState->setText("Boot State: Normal Boot");
            inFastboot = false;
        } else {
            // Timeout occurred
            showCustomAlert("Error", "Fastboot is waiting for a device to connect.\nMake sure it's properly connected and try again.");
            ui->detectFastbootLabel->setStyleSheet("color: red;");
        }
    } else {
        // Failed to start the process
        QMessageBox::information(this, "Error", "Failed to start process.");
    }
}


void MainWindow::on_detectFastboot_clicked()
{
    QString deviceID;

    QProcess process;
    process.setWorkingDirectory("platform-tools");
    process.start("cmd.exe", QStringList() << "/c" << "fastboot device");  // Run "adb devices" command in cmd.exe

    if (process.waitForStarted()) {
        // Wait for the process to finish or timeout after 2 seconds
        if (process.waitForFinished(2000)) {
            QString output = process.readAllStandardOutput();  // Read the command output

            // Extract device ID
            QString startText = "";
            QString endText = "\tfastboot\r\n";
            int startIndex = output.indexOf(startText) + startText.length();
            int endIndex = output.indexOf(endText);
            deviceID = output.mid(startIndex, endIndex - startIndex);

            ui->detectFastbootLabel->setText("Device Connected!\nDevice ID: " + deviceID);
            ui->detectFastbootLabel->setStyleSheet("color: green;");
            ui->exitBootloader->setEnabled(true);
            inFastboot = true;
            ui->deviceIDLabel->setStyleSheet("color: green;");
            ui->deviceIDLabel->setText("Device ID:" + deviceID);
        } else {
            // Timeout occurred
            showCustomAlert("Error", "Fastboot is waiting for a device to connect.\nMake sure it's properly connected and try again.");
            ui->detectFastbootLabel->setStyleSheet("color: red;");
        }
    } else {
        // Failed to start the process
        QMessageBox::information(this, "Error", "Failed to start process.");
    }
}

void MainWindow::on_binBtn_clicked()
{
    ui->filePathBtn->setEnabled(true);
    ui->extractPayload->setText("Extract boot image");
    ui->extractPayload->setEnabled(true);
}

void MainWindow::on_zipBtn_clicked()
{
    ui->extractPayload->setText("Extract payload");
    ui->filePathBtn->setEnabled(true);
    ui->extractPayload->setEnabled(true);
}

void MainWindow::extractFileFromZip(const QString& zipFilePath, const QString& filePathInZip, const QString& outputPath)
{
    QFile zipFile(zipFilePath);
    if (!zipFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, "Error", "Failed to open the zip archive file for reading.");
        return;
    }

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, "Error", "Failed to open the output file for writing.");
        return;
    }

    QByteArray fileData = zipFile.readAll();
    QString fileDataString = QString::fromUtf8(fileData);

    int startIndex = fileDataString.indexOf(filePathInZip);
    if (startIndex == -1)
    {
        QMessageBox::information(this, "Error", "payload.bin was not found in the zip archive.");
        return;
    }

    QByteArray extractedData = fileData.mid(startIndex + filePathInZip.length());

    outputFile.write(extractedData);
    outputFile.close();

    QMessageBox::information(this, "Success", "File extracted successfully.");
}



void MainWindow::on_filePathBtn_clicked()
{
    QString patchFilePath;

    if (ui->binBtn->isChecked())
    {
        patchFilePath = QFileDialog::getOpenFileName(this, "Select Patch File", "", "Binary Files (*.bin)");
    }
    else if (ui->zipBtn->isChecked())
    {
        patchFilePath = QFileDialog::getOpenFileName(this, "Select Patch File", "", "Zip Files (*.zip)");
    }
    else if (ui->imgBtn->isChecked())
    {
        patchFilePath = QFileDialog::getOpenFileName(this, "Select Patch File", "", "Image Files (*.img)");
    }
    else
    {
        return;
    }

    if (!patchFilePath.isEmpty())
    {
        ui->filePath->setText(patchFilePath);
    }
}

void MainWindow::disableAllBtn(bool disable)
{
    if(disable)
    {
        ui->detectFastboot->setEnabled(false);
        ui->exitBootloader->setEnabled(false);
        ui->bootFastboot->setEnabled(false);
        ui->detectADBDevice->setEnabled(false);
        ui->extractPayload->setEnabled(false);
        ui->binBtn->setEnabled(false);
        ui->zipBtn->setEnabled(false);
        ui->imgBtn->setEnabled(false);
        ui->filePath->setEnabled(false);
        ui->filePathBtn->setEnabled(false);
        ui->pushIMG->setEnabled(false);
        ui->pullIMG->setEnabled(false);
    }
    else if (!disable)
    {
        ui->detectFastboot->setEnabled(true);
        ui->bootFastboot->setEnabled(true);
        ui->detectADBDevice->setEnabled(true);
        ui->extractPayload->setEnabled(true);
        ui->binBtn->setEnabled(true);
        ui->zipBtn->setEnabled(true);
        ui->imgBtn->setEnabled(true);
        ui->filePath->setEnabled(true);
        ui->filePathBtn->setEnabled(true);
        ui->pushIMG->setEnabled(true);
        ui->pullIMG->setEnabled(true);
    }
}

void MainWindow::on_extractPayload_clicked()
{
    if(ui->filePath->text() == "")
        return;
    if (ui->zipBtn->isChecked())
    {

        QString filePath = ui->filePath->text();
        QString outputFileName = "payload.bin";

        QString projectDir = QCoreApplication::applicationDirPath();
        QString outputPath = projectDir + QDir::separator() + outputFileName;

        QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]()
                {
                    loadingTimer->stop();
                    onExtractionCompleted();
                    watcher->deleteLater();
                });

        QFuture<void> future = QtConcurrent::run([filePath, outputPath, this]()
                                                 {
                                                     extractFileFromZip(filePath, "payload.bin", outputPath);
                                                 });

        watcher->setFuture(future);

        QTimer* loadingTimer = new QTimer(this);
        connect(loadingTimer, &QTimer::timeout, [this]() // Store the loadingTimer
                {
                    QString loadingText = "Extracting";
                    for (int i = 0; i < dotCount; i++)
                    {
                        loadingText += ".";
                    }
                    ui->extractPayload->setText(loadingText);
                    dotCount = (dotCount % 5) + 1;

                });
        loadingTimer->start(500);
        this->loadingTimer = loadingTimer; // Store the loadingTimer in the class member
    }
    if (ui->binBtn->isChecked()){
        QString filePath = ui->filePath->text();
        QString executablePath = "C:/Users/Simon/OneDrive - EBUR/OOP/AndRootV2/payload-dumper-go.exe";
        QProcess process;
        process.start(executablePath, QStringList() << filePath);
        process.waitForFinished();  // Wait for the process to finish
        QString output = process.readAllStandardOutput();  // Read the command output
        if(output.isEmpty()){
            ui->ImagePathLabel->setStyleSheet("color: red;");
            ui->ImagePathLabel->setText("Something went wrong!");
        } else {
            ui->ImagePathLabel->setStyleSheet("color: green;");
            ui->ImagePathLabel->setText("Image file extracted!");
            ui->openImagePath->setEnabled(true);
            // Extract Link
            QString startText = "payload.bin: ";
            QString endText = "/payload.bin";
            int startIndex = output.indexOf(startText) + startText.length();
            int endIndex = output.indexOf(endText);
            imagePath = output.mid(startIndex, endIndex - startIndex);
        }
    }
    if (ui->imgBtn->isChecked())
    {
        disableAllBtn(true);
        QMessageBox::information(this, "Note", "This will run 'fastboot BOOT boot.img'. It'll not flash the image directly.\n\nThat means it'll boot into a temporary rooted state, and you'll have to manually flash it by opening Magisk->Install->Direct Install. This is to prevent bootloops if anything is wrong with the image file.");

        QString imgPath = ui->filePath->text();

        QProcess process;
        process.setWorkingDirectory("platform-tools");
        process.start("cmd.exe", QStringList() << "/c" << "fastboot boot " << imgPath);
        process.waitForFinished();

        disableAllBtn(false);
        QMessageBox::information(this, "Success!", "If you enter a bootloop just hold power+volume(up/down) until the phone restarts. If it boots successfully and you want to make the root permanent:\n\nMagisk->Install->Direct Install(Recommended)->Reboot");
    }
}

void MainWindow::onExtractionCompleted()
{
    ui->extractPayload->setText("Extraction completed!");
    disableAllBtn(false);
    loadingTimer->disconnect(); // Disconnect the loadingTimer
    loadingTimer->deleteLater(); // Delete the loadingTimer
    ui->filePath->setText("");
}

void MainWindow::on_openImagePath_clicked()
{
    QUrl url = QUrl::fromLocalFile(imagePath);
    QDesktopServices::openUrl(url);
}


void MainWindow::on_imgBtn_clicked()
{
    ui->openImagePath->setEnabled(true);
    ui->extractPayload->setText("Boot image");
    ui->filePath->setText("");
    if(!inFastboot)
        ui->extractPayload->setEnabled(false);
    else
        ui->extractPayload->setEnabled(true);
}

void MainWindow::on_USBDebugging_stateChanged()
{
    if(ui->USBDebugging->isChecked() && ui->backedUp->isChecked() && ui->bootloaderUnlocked->isChecked())
        disableAllBtn(false);
    else
        disableAllBtn(true);
}


void MainWindow::on_backedUp_stateChanged()
{
    if(ui->USBDebugging->isChecked() && ui->backedUp->isChecked() && ui->bootloaderUnlocked->isChecked())
        disableAllBtn(false);
    else
        disableAllBtn(true);
}


void MainWindow::on_bootloaderUnlocked_stateChanged()
{
    if(ui->USBDebugging->isChecked() && ui->backedUp->isChecked() && ui->bootloaderUnlocked->isChecked())
        disableAllBtn(false);
    else
        disableAllBtn(true);
}

void MainWindow::on_info_clicked()
{
    QMessageBox::information(this, "AndRoot V3.0", "Info\nInfo\nInfo");
}


void MainWindow::on_pushIMG_clicked()
{
    QFileDialog fileDialog;
    fileDialog.setWindowTitle("Open Image File");
    fileDialog.setFileMode(QFileDialog::ExistingFile); // Allow selecting an existing file
    fileDialog.setNameFilter("IMG Files (*.img)");
    fileDialog.setWindowFlags(Qt::FramelessWindowHint);

    // Show the dialog and get the selected file
    if (fileDialog.exec() == QDialog::Accepted)
    {
        QString selectedFile = fileDialog.selectedFiles().first(); // Get the first selected file

        // Create a custom QMessageBox
        QMessageBox msgBox;
        msgBox.setWindowTitle("Push File");
        msgBox.setText("Selected file:\n\n" + selectedFile);
        msgBox.setIcon(QMessageBox::Information);

        msgBox.addButton("Continue", QMessageBox::AcceptRole); // Add "Continue" button
        msgBox.addButton("Cancel", QMessageBox::RejectRole); // Add "Cancel" button

        msgBox.setStyleSheet("QMessageBox {"
                             "background-color: white;" // Background color
                             "border: 2px solid black;"   // Border style
                             "border-radius:5px;"
                             "}"
                             "QLabel {"
                             "color: #333;"              // Text color
                             "font-size: 16px;"          // Font size
                             "}"
                             "QPushButton {"
                             "background-color: rgba(250,0,99,0.85);" // Button background color
                             "color: black;"               // Button text color
                             "border: 2px solid black;"    // Remove button borders
                             "border-radius: 15px;"
                             "padding: 5px 10px;"         // Button padding
                             "}"
                             "QPushButton:hover {"
                             "background-color: #005eaa;" // Hover effect
                             "}");

        // Execute the QMessageBox and handle button clicks
        int result = msgBox.exec();
        if (result == QMessageBox::AcceptRole)
        {
            QProcess process;
            process.setWorkingDirectory("platform-tools");
            process.setReadChannel(QProcess::StandardOutput); // Set the read channel to standard output

            QStringList arguments;
            arguments << "/c" << "adb push" << selectedFile << "/sdcard/"; // Add your adb push command and arguments

            process.start("cmd.exe", arguments);
            process.waitForFinished();

            QString output = process.readAllStandardOutput();
            QString error = process.readAllStandardError();

            if(!output.isEmpty())
            {
                QMessageBox::information(this, "Done.", "File successfully pushed to device.\n\n", output);
            }
            else
            {
                QMessageBox::information(this, "Could't push file.", "Make sure your device is properly connected and try again.\n\n", error);
            }
        }
        else if (result == QMessageBox::RejectRole)
        {
            //Handle cancelled action
        }
    }
}


void MainWindow::on_pullIMG_clicked()
{
    // Create a custom QMessageBox
    QMessageBox msgBox;
    msgBox.setWindowTitle("Pull File");
    msgBox.setText("The file will be automatically pulled from the Download folder.\n\nMake sure the patched image is there and there are no other magisk patched images in the same folder.");
    msgBox.setIcon(QMessageBox::Information);

    msgBox.addButton("Continue", QMessageBox::AcceptRole); // Add "Continue" button
    msgBox.addButton("Cancel", QMessageBox::RejectRole); // Add "Cancel" button

    msgBox.setStyleSheet("QMessageBox {"
                         "background-color: white;" // Background color
                         "border: 2px solid black;"   // Border style
                         "border-radius:5px;"
                         "}"
                         "QLabel {"
                         "color: #333;"              // Text color
                         "font-size: 16px;"          // Font size
                         "}"
                         "QPushButton {"
                         "background-color: rgba(250,0,99,0.85);" // Button background color
                         "color: black;"               // Button text color
                         "border: 2px solid black;"    // Remove button borders
                         "border-radius: 15px;"
                         "padding: 5px 10px;"         // Button padding
                         "}"
                         "QPushButton:hover {"
                         "background-color: #005eaa;" // Hover effect
                         "}");

    // Execute the QMessageBox and handle button clicks
    int result = msgBox.exec();
    if (result == QMessageBox::AcceptRole)
    {
        QProcess process;
        process.setWorkingDirectory("platform-tools");
        QString filePath;

        // Execute adb shell command to find the image file
        QString adbCommand = "adb shell find /sdcard/Download/ -type f -name 'magisk_patched*.img'";
        process.setProgram("cmd.exe");
        process.setArguments({ "/C", adbCommand });
        process.start();
        process.waitForFinished(-1);

        if (process.exitCode() == 0)
        {
            filePath = process.readAllStandardOutput().trimmed();

            qDebug() << "Filepath" << filePath;

            if (process.exitCode() == 0)
            {
                QTextStream stream(&process);
                while (!stream.atEnd())
                {
                    QString line = stream.readLine().trimmed();
                    if (!line.isEmpty())
                    {
                        filePath = line;
                        break; // Stop processing after finding the first matching file
                    }
                }
            }

            // Check if filePath is not empty
            if (!filePath.isEmpty())
            {
                // Extract the filename from the full path
                QString fileName = QFileInfo(filePath).fileName();

                // Execute adb pull command to pull the image file

                QProcess process;
                process.setWorkingDirectory("platform-tools");
                process.setReadChannel(QProcess::StandardOutput); // Set the read channel to standard output

                QStringList arguments;
                arguments << "/c" << "adb pull" << filePath; // Add your adb push command and arguments

                process.start("cmd.exe", arguments);
                process.waitForFinished();

                if (process.exitCode() == 0)
                {
                    qDebug() << "Successfully pulled image file:" << fileName;
                    QMessageBox::information(this, "Done.", "Successfully pulled image file: " ,fileName, " to the folder 'platform-tools'.");
                }
                else
                {
                    qDebug() << "Failed to pull image file:" << fileName << "with exit code:" << process.exitCode();
                    QMessageBox::information(this, "Failed to pull file.", "Failed to pull image file with exit code: ", process.exitCode());
                }
            }
            else
            {
                qDebug() << "No image file found with the specified prefix.";
                QMessageBox::information(this, "Failed to pull file.", "No image file found with the specified prefix.");
            }
        }
        else
        {
            qDebug() << "Failed to execute adb shell command with exit code:" << process.exitCode();
            QMessageBox::information(this, "Failed to pull file.", "Failed to execute adb shell command with exit code: ", process.exitCode());
        }
    }
    else if (result == QMessageBox::RejectRole)
    {
        //Handle cancelled action
    }
}

