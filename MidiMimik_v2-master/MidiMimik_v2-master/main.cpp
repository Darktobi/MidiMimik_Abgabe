#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <string>
#include <stdio.h>
#include <QTableWidgetItem>
#include "mainwindow.h"
#include "midiinput.h"

using namespace drumstick::rt;
using namespace cv;
using namespace std;


//Absoluter Pfad, entweder auch absolut angeben oder ausprobieren

String face_cascade_name = "C:/Users/Nick/Projekte/Uni/WiSe16/AVPRG/MidiMimik/cascades/haarcascade_frontalface_default.xml";
String eyes_cascade_name = "C:/Users/Nick/Projekte/Uni/WiSe16/AVPRG/MidiMimik/cascades/ojoD.xml";


CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;


class Melodythread : public QThread {
public:
    MIDIOutput out;
public:
    bool abort = false;
public:
    int bassVel = 10;
public:
    int hihatVel = 10;
public:
    int snareVel = 10;
public:
    int guitarVel = 10;
public:
    int speed = 300;


public: void run() {

        while(!abort){
            for(int i = 0; i < 2; i++){
                out.sendProgram(0, 28);
                out.sendNoteOn(9, 42, hihatVel);
                out.sendNoteOn(0, 47, guitarVel);
                this->msleep(speed);
                out.sendNoteOn(0, 54, guitarVel);
                out.sendNoteOn(9, 42, hihatVel);
                out.sendNoteOn(9, 36, bassVel);
                this->msleep(speed);
                out.sendNoteOn(0, 47, guitarVel);
                out.sendNoteOn(9, 42, hihatVel);
                this->msleep(speed);
                out.sendNoteOn(0, 50, guitarVel);
                out.sendNoteOn(9, 42, hihatVel);
                out.sendNoteOn(9, 38, snareVel);
                this->msleep(speed);
                out.sendNoteOn(9, 42, hihatVel);
                this->msleep(speed);
                out.sendNoteOn(9, 42, hihatVel);
                out.sendNoteOn(9, 36, bassVel);
                this->msleep(speed);
                out.sendNoteOn(9, 42, hihatVel);
                this->msleep(speed);
                out.sendNoteOn(9, 42, hihatVel);
                out.sendNoteOn(9, 38, snareVel);
                this->msleep(speed);
                out.sendNoteOn(0, 43, guitarVel);
                out.sendNoteOn(9, 42, hihatVel);
                this->msleep(speed);
                out.sendNoteOn(0, 54, guitarVel);
                out.sendNoteOn(9, 42, hihatVel);
                out.sendNoteOn(9, 36, bassVel);
                this->msleep(speed);
                out.sendNoteOn(0, 43, guitarVel);
                out.sendNoteOn(9, 42, hihatVel);
                this->msleep(speed);
                out.sendNoteOn(0, 50, guitarVel);
                out.sendNoteOn(9, 42, hihatVel);
                out.sendNoteOn(9, 38, snareVel);
                this->msleep(speed);
            }
            out.sendNoteOn(9, 42, hihatVel);
            this->msleep(speed);
            out.sendNoteOn(9, 42, hihatVel);
            out.sendNoteOn(9, 36, bassVel);
            out.sendNoteOn(0, 55, guitarVel);
            this->msleep(speed);
            out.sendNoteOn(9, 42, hihatVel);
            this->msleep(speed);
            out.sendNoteOn(9, 42, hihatVel);
            out.sendNoteOn(9, 38, snareVel);
            this->msleep(speed);
        }
    }
};

double getEyeQuotient(Mat equalizedInput){

    //Rechenwerte anlegen
    int numBlackRows = 0;
    int numBlackRowsMax  = 0;
    int numBlackCols  = 0;

    //Für alle Spalten
    for(int x = 0; x < equalizedInput.cols; x++){
        if (numBlackRows >= 1){
            numBlackCols++;
        }
        numBlackRows = 0;

        //Für alle Reihen
        for(int y = 0; y < equalizedInput.rows; y++){
            //Unser Pixel an der Stelle
            Vec3b inputPixel = equalizedInput.at<Vec3b>(y, x);
            //inputPixel beinhaltet Werte für alle 3 Farben, wir brauchen aber nur eine da wir ein Binärbild kriegen
            int blue = inputPixel[0];

            //Falls der Pixel schwarz ist
            if(blue == 0){
                 numBlackRows++;
            }
            if (numBlackRows > numBlackRowsMax){
                 numBlackRowsMax = numBlackRows;
            }
        }
    }

    //Quotienten anlegen und größeren Wert durch den kleineren teilen
    double eyeQuotient;

    if(numBlackRowsMax >= numBlackCols){
        eyeQuotient = (double)numBlackCols/numBlackRowsMax;
    } else{
        eyeQuotient = (double)numBlackRowsMax/numBlackCols;
    }

    return eyeQuotient;

}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Wichtiges Objekt, w ist unser Control Panel, hieraus holen wir diverse Werte
    MainWindow w;
    //400 und 200 sind die Startkoordinaten, das andere sind die Maße
    w.setGeometry(400, 200, 562, 434);
    w.show();

    //Load Cascades
    face_cascade.load(face_cascade_name);
    eyes_cascade.load(eyes_cascade_name);

    //initializing
    VideoCapture cap;
    vector<Rect> faces;

    Mat leftEye;
    Mat rightEye;
    Mat equalizedLeftEye;
    Mat equalizedRightEye;

    Melodythread drums;

    vector<string> s = {"Bass", "HiHat", "Snare", "Guitar", "Delay"};

    bool instrumentChanged = false;
    bool volumeChanged = false;

    //Alle MidiOutputs die wir haben auslesen, nur für uns
    QStringList outputConnections = drums.out.connections(true);
    qDebug() << "MIDI Output Connections:";
    for (int i = 0; i < outputConnections.size(); ++i){
        qDebug() << outputConnections.at(i);
    }
    //Wichtig! Gewünschten Midioutput öffnen, hieraus spielen wir unsere Töne ab
    drums.out.open("Microsoft GS Wavetable Synth");

    double quotientLeft;
    double quotientRight;

    if(face_cascade.load(face_cascade_name) == true && eyes_cascade.load(eyes_cascade_name) == true){
        qDebug() << "success loading";

        //closedCounter zählen Frames in denen hintereinander das Auge geschlossen war, müssen nach jedem Frame auf 0 gesetzt werden
        int closedCounterLeft = 0;
        int closedCounterRight = 0;
        int openCounterLeft = 0;
        int openCounterRight = 0;
        int trackframceounter = 0;

        //Grenzwert ab wann ein Auge als geschlossen gilt
        double quotientThresh = 0.3;

        //Zahl anpasen je nach Kamera
        if(!cap.open(0))
            return 0;
        for(;;)
        {
            //Frame für Kamerabild
            Mat frame;
            cap >> frame;
            //Wenn kein Bild, brich ab
            if(frame.empty()) break;

            //Graubild
            Mat frame_gray;
            cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

            //Histogramm
            equalizeHist(frame_gray, frame_gray);

            //Get Faces
            face_cascade.detectMultiScale( frame_gray, faces, 1.3, 3);


            //Für alle Gesichter

            for( size_t i = 0; i < faces.size(); i++ )
            {
                //rectangle(frame, Rect(Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height/2)),(255, 0, 0), 3);
                rectangle(frame, faces[i], Scalar(119, 181, 201), 2);
                //Nur der Gesichtsbereich ROI = Region of Interest
                Mat faceROI = frame_gray( faces[i] );

                vector<Rect> eyes;
                //-- In each face, detect eyes
                eyes_cascade.detectMultiScale( faceROI, eyes, 1.05, 100);

                //Wenn nur ein oder kein Auge zu sehen ist, halt den Thread an nach 10 Frames
                if(eyes.size() == 0 || eyes.size() == 1){
                    trackframceounter++;
                    if(drums.isRunning() && trackframceounter >= 10){
                        drums.abort = true;
                    }
                }
                //Wenn wir 2 Augen sehen, guck erst ob der Thread schon läuft, wenn ja gucken wir ob er das erste Mal gestartet wird
                //oder er schonmal abgebrochen wurde, dann setzen wir den counter zurück und lassen ihn weiterlaufen

                else if(eyes.size() == 2){
                    if(!drums.isRunning()){
                        if(drums.abort == true){
                            trackframceounter = 0;
                            drums.abort = false;
                        } else{
                        drums.start();
                        }
                    }
                }
                //Die Lautstärken holen wir uns aus dem Thread deswegen an dieser Stelle
                int volumes[5] = {drums.bassVel, drums.hihatVel, drums.snareVel, drums.guitarVel, drums.speed};

                //Für alle gefundenen Augen
                for( size_t j = 0; j < eyes.size(); j++ )
                {

                    //Radius
                    int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );

                    //Wenn genau zwei Augen gefunden worden sind und der Index auf 0
                    if(j == 0 && eyes.size() == 2){

                        //Mittelpunkte der beiden gefundenen Augen
                        Point eye_center1( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
                        Point eye_center2( faces[i].x + eyes[j+1].x + eyes[j+1].width/2, faces[i].y + eyes[j+1].y + eyes[j+1].height/2 );

                        //Region of interest der beiden Augen
                        Rect leftROI(eye_center1.x-radius, eye_center1.y-radius, radius*2, radius*2);
                        Rect rightROI(eye_center2.x-radius, eye_center2.y-radius, radius*2, radius*2);


                        //Auge links oder Recht Bestimmung
                        if(eye_center1.x < eye_center2.x){
                            leftEye = frame(leftROI);
                            rightEye = frame(rightROI);

                        }
                        else if(eye_center1.x > eye_center2.x){
                            rightEye = frame(leftROI);
                            leftEye = frame(rightROI);
                            Point temp = eye_center2;
                            eye_center2 = eye_center1;
                            eye_center1 = temp;
                        }

                        //Mach ein Binärbild
                        cvtColor(leftEye, leftEye, CV_BGR2GRAY);
                        equalizeHist(leftEye, equalizedLeftEye);
                        threshold(equalizedLeftEye, equalizedLeftEye, w.threshhold, 255, THRESH_BINARY);
                        medianBlur(equalizedLeftEye, equalizedLeftEye, 5);

                        cvtColor(rightEye, rightEye, CV_BGR2GRAY);
                        equalizeHist(rightEye, equalizedRightEye);
                        threshold(equalizedRightEye, equalizedRightEye, w.threshhold, 255, THRESH_BINARY);
                        medianBlur(equalizedRightEye, equalizedRightEye, 5);


                        //Lass den Quotienten aus schwarzen Pixel Reihen und schwarzen Pixel Spalten berechnen
                        quotientLeft = getEyeQuotient(equalizedLeftEye);
                        quotientRight = getEyeQuotient(equalizedRightEye);

                        //Zeig die beiden Binärbilder
                        imshow("equalizedLeft", equalizedLeftEye);
                        imshow("equalizedRight", equalizedRightEye);

                        //Wenn das linke Auge geschlossen ist
                        if(quotientLeft <= quotientThresh){
                            //Zähl einen Frame hoch
                            closedCounterLeft++;
                            openCounterLeft = 0;
                            //Wenn 3 frames hintereinander gefunden worden sind, zeichnen wir den Text aufs Bild
                            if(closedCounterLeft >= 3){
                               putText(frame, "Closed", Point(eye_center1.x-50, eye_center1.y-50), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255, 0, 0), 1);
                               cout <<"\nLeft Eye closed";
                               if(closedCounterLeft >= 5 && instrumentChanged == false){
                                   //Ein Instrument weiterschalten
                                   w.instrumentIndex++;
                                   //Es gibt kein 6. Instrument das heißt wir fangen wieder von vorne an
                                   if(w.instrumentIndex == 5) w.instrumentIndex = 0;
                                   instrumentChanged = true;
                               }
                            }

                        } else {
                            closedCounterLeft = 0;
                            openCounterLeft++;
                            if(openCounterLeft >= 5){
                                instrumentChanged = false;
                            }
                            putText(frame, "Open", Point(eye_center1.x-50, eye_center1.y-50), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255, 0, 0), 1);
                        }
                        //Wenn das rechte Auge nach unseren Berechnungen geschlossen ist
                        if(quotientRight <= quotientThresh){
                            //Hier zählen wir wieder Frames
                            closedCounterRight++;
                            //Nach 3 Frames sagen wir das Auge ist sicher geschlossen
                            if(closedCounterRight >= 3 && closedCounterLeft <= 3){
                                putText(frame, "Closed", Point(eye_center2.x - 50, eye_center2.y-50), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255, 0, 0), 1);
                                cout <<"\nRight Eye closed";
                                //Je nachdem welches Instrument wir gerade bearbeiten schalte die entsprechende Lautstärke höher oder tiefer
                                //Abhängig davon ob die Checkbox "Turn upward" gecheckt ist oder nicht (turnUp)
                                if(closedCounterRight >= 5){
                                    if(w.instrumentIndex == 0){
                                        closedCounterRight = 0;
                                        if(w.turnUp){
                                            drums.bassVel += 10;
                                        } else{
                                            drums.bassVel -= 10;
                                        }
                                        volumeChanged = true;
                                        if(drums.bassVel == 130){
                                            drums.bassVel = 10;
                                        }else if(drums.bassVel == 0){
                                            drums.bassVel =120;
                                        }
                                        //In das Feld in der Liste soll ein String hier konvertieren wir nur die Zahl
                                        w.bassString = QString::number(drums.bassVel);
                                    }
                                    else if(w.instrumentIndex == 1){
                                        closedCounterRight = 0;
                                        if(w.turnUp){
                                            drums.hihatVel += 10;
                                        } else{
                                            drums.hihatVel -= 10;
                                        }
                                        volumeChanged = true;
                                        if(drums.hihatVel == 130){
                                            drums.hihatVel = 10;
                                        }else if(drums.hihatVel == 0){
                                            drums.hihatVel =120;
                                        }
                                        w.hihatString = QString::number(drums.hihatVel);
                                    }
                                    else if(w.instrumentIndex == 2){
                                        closedCounterRight = 0;
                                        if(w.turnUp){
                                            drums.snareVel += 10;
                                        } else{
                                            drums.snareVel -= 10;
                                        }
                                        volumeChanged = true;
                                        if(drums.snareVel == 130){
                                            drums.snareVel = 10;
                                        }else if(drums.snareVel == 0){
                                            drums.snareVel =120;
                                        }
                                        w.snareString = QString::number(drums.snareVel);
                                    }
                                    else if(w.instrumentIndex == 3){
                                        closedCounterRight = 0;
                                        if(w.turnUp){
                                            drums.guitarVel += 10;
                                        } else{
                                            drums.guitarVel -= 10;
                                        }
                                        volumeChanged = true;
                                        if(drums.guitarVel == 130){
                                            drums.guitarVel = 10;
                                        }else if(drums.guitarVel == 0){
                                            drums.guitarVel =120;
                                        }
                                        w.guitarString = QString::number(drums.guitarVel);
                                    }
                                    else if(w.instrumentIndex == 4){
                                        closedCounterRight = 0;
                                        if(w.turnUp){
                                            drums.speed += 25;
                                        } else{
                                            drums.speed -= 25;
                                        }
                                        volumeChanged = true;
                                        if(drums.speed == 425){
                                            drums.speed = 200;
                                        } else if(drums.speed == 175){
                                            drums.speed = 400;
                                        }
                                        w.speedString = QString::number(drums.speed);
                                    }
                                }
                            }

                        } else{
                            closedCounterRight = 0;
                            openCounterRight++;
                            if(openCounterRight >= 5){
                                volumeChanged = false;
                            }
                            putText(frame, "Open", Point(eye_center2.x-50, eye_center2.y-50), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255, 0, 0), 1);
                            //cout<<"\nRight Eye open";
                        }

                        //Mal die blauen Kreise um die Augen
                        circle(frame, eye_center1, radius, Scalar( 119, 181, 201 ), 2, 8, 0 );
                        circle(frame, eye_center2, radius, Scalar( 119,  181, 201), 2, 8, 0);

                        putText(frame, s[w.instrumentIndex], Point(50, 50), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255, 0, 0), 1);
                        putText(frame, to_string(volumes[w.instrumentIndex]), Point(50, 100), FONT_HERSHEY_COMPLEX, 1.0, Scalar(255, 0, 0), 1);
                        //Refresh Funktion damit die Werte im Fenster immer aktuell sind
                        w.refresh();
                    }
                }
            }

            //MainFrame
            imshow( "Camera", frame);

            //key 27 = ESC
            if( waitKey(1) == 27 ) break;
         }


     } else {
        qDebug() << "Kaskaden nicht richtig geladen, Pfad gecheckt?";
    }

    return 0;
}

