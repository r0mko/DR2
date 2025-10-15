import QtQuick 2.5
import QtQuick.Window 2.2
import Test 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.Graphics 1.0
import Dr.UI 1.0
Window {
    id: root
    width: 1000
    height: 620
    visible: true
    title: "Падение напряжения на участке 5602"


    Window {
        title: 'Трамвай #2088'
        visible: true
        width: 400
        height: 367
        color: '#ccc'

        ColumnLayout {
            anchors.fill: parent
            Rectangle {
                Layout.fillWidth: true
                Layout.minimumHeight: img.implicitHeight
                color: '#ccc'
                Image{
                    id: img
                    width: parent.width
                    fillMode: Image.PreserveAspectFit
                    source: '/Users/Tox/Downloads/tram.png'
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: '#ccc'
                JSObjectView {
                    anchors.fill: parent

                    //            onImplicitHeightChanged: { guide_panel.contentHeight = implicitHeight + 4 }
                    JSObjectViewRow {
                        text: "id"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                                text: '2088'
                            }

                        }
                    }
                    JSObjectViewRow {
                        text: "Номер маршрута"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                                text: '9'
                            }

                        }
                    }
                    JSObjectViewRow {
                        text: "Тип"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                text: 'Трамвай'
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Модель"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                text: 'КТМ-19КТ'
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Масса порожнего вагона"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                text: '19.5 т'
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Постоянная мощность тяговых двигателей"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                text: '50 кВт'
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Скорость"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                text: '15 км/ч'
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Ускорение"
                        valueDelegate: Rectangle {
                            height: 20
                            Text {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                                text: '1.0 м/с²'
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Расчётная мощность"
                        valueDelegate: Rectangle {
                            height: 20
                            color: '#ddf'
                            Text {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                                text: '81 кВт'
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Расчётный ток"
                        valueDelegate: Rectangle {
                            height: 20
                            color: '#ddf'
                            Text {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                                text: '150 А'
                            }
                        }
                    }
                    JSObjectViewRow {
                        text: "Расчётное напряжение"
                        valueDelegate: Rectangle {
                            height: 20
                            color: '#ddf'
                            Text {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                verticalAlignment: Text.AlignVCenter
                                text: '540.31 В' //'524,06 В'
                            }
                        }
                    }
                }
            }
        }
    }


    property real l1: 962.0
    property real l2: 513.0
    property real l3: l1 + l2


    property var ps1: [214.0/l1, 530.0/l1, 750.0/l1]
    property var ps2: [(l2-405.0)/l2, (l2-206.0)/l2]
    property var ps3: [214.0/(l1+l2), 530.0/(l1+l2), 750.0/(l1+l2), (l1+l2-405.0)/(l1+l2), (l1+l2-206.0)/(l1+l2)]



    property var tr1: [ 9, 19, 9 ]
    property var tr2: [ 19, 9 ]
    property var tr3: [ 9, 19, 9, 9, 19 ]


    Component.onCompleted: {
        ps3.forEach( function(e){console.log( e )})
    }

    property var eI1: []
    property var eI2: []
    property var eI3: []





    property real r: 0.0177 * 1e-3


    function gen(){

        var max = 40.0
        eI1[0] = 1 * max
        eI1[1] = 0.3 * max
        eI1[2] = 0.6 * max


        eI2[1] = 0.1 * max
        eI2[0] = 0.5 * max
        //        ps1.forEach(function(e, i){
        //            eI1[i] = (Math.random()*40 + 20)*0.5
        //        })
        //        ps2.forEach(function(e, i){
        //            eI2[i] = (Math.random()*40 + 20)*0.5
        //        })
        eI3[0] = eI1[0]
        eI3[1] = eI1[1]
        eI3[2] = eI1[2]
        eI3[3] = eI2[1]
        eI3[4] = eI2[0]

    }

    Action {
        shortcut: 'G'
        onTriggered: {
            gen()
            c1.requestPaint()
            c2.requestPaint()
            c3.requestPaint()
        }
    }

    Action {
        shortcut: 'F'
        property int s: 0
        onTriggered: {
            if(s === 0){
                c1.visible = true
                c2.visible = false
                c3.visible = false
                root.title = 'Падение напряжения на участке 5602 - Нормальный режим'
                s++
            }else if(s === 1){
                c1.visible = false
                c2.visible = true
                c3.visible = false
                root.title = 'Падение напряжения на участке 5605 - Нормальный режим'
                s++
            }else if(s === 2){
                c1.visible = false
                c2.visible = false
                c3.visible = true
                root.title = 'Падение напряжения на участке 5602 - 5605 - Вынужденный режим'
                s = 0
            }
        }
    }

    Canvas {
        id: c1
        anchors.fill: parent
        onPaint: {

            var points = []
            var c = getContext('2d')
            c.strokeStyle = '#000000'
            c.clearRect(0, 0, width, height);



            c.textAlign="center";
            c.font = '34px Roboto'
            c.globalAlpha=0.3;
            c.fillText('Нормальный режим', width * 0.5, 70)
            c.globalAlpha=1;
            c.textAlign="start";
            c.font = '14pt Roboto'



            c.beginPath();
            c.moveTo(30.5,height);
            c.lineTo(30.5,30);
            c.lineWidth = 0.3;
            c.stroke();


            c.beginPath();
            c.moveTo(25.5, height - 30.5 );
            c.lineTo(width - 30, height - 30.5);
            c.lineWidth = 0.3;
            c.stroke();

            c.font = '14pt Roboto'
            c.fillText('U, В', 19, 17)


            var v400 = height - 30.5
            var v650 = 29.5
            var v600 = v400 - (v400 - v650) * 0.8
            var v420 = v400 - (v400 - v650) * 0.12
            var v510 = v400 - (v400 - v650) * 0.44



            var vtos = 4

            c.fillText('400', 4, v400 + vtos )

            //            c.beginPath();
            //            c.moveTo(25.5,v420);
            //            c.lineTo(width - 30,v420);
            //            c.lineWidth = 0.5;
            //            c.stroke();
            //            c.fillText('430', 4, v420+vtos)

            c.beginPath();
            c.moveTo(25.5,v510);
            c.lineTo(width - 30,v510);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('510', 4, v510+vtos)

            c.beginPath();
            c.moveTo(25.5,v600);
            c.lineTo(width - 30,v600);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('600', 4, v600+vtos)

            c.beginPath();
            c.moveTo(25.5,v650);
            c.lineTo(width - 30,v650);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('650', 4, v650+vtos)


            c.fillText('%1 м'.arg(l1), width - 65, height - 10 )


            var k =  0.93
            var y = v600 + ( height - 60 ) * (1 - k)
            var y1 = y
            c.beginPath();
            c.moveTo(30.5, y);
            for( var i = 0; i < ps1.length; ++i ) {
                var I = getI(eI1, i)

                var ll0 = i == 0 ? 0 : ps1[ i - 1 ]
                var ll1 = ps1[ i ]
                var dv = I*I*r*l1*(ll1-ll0)

                y = y + dv * (height-60)/250

                c.lineTo(gx(ps1[i],l1), y + ( height - 60 ) * (1 - k));
                var vv = -20+400 + 250 * (height - y-30) / (height - 60)
                points[ i ] = { 'x': gx(ps1[i],l1), 'y': y + ( height - 60 ) * (1 - k), 'v':~~(vv), 'i': I }

            }

            c.lineTo(width - 30,y+ ( height - 60 ) * (1 - k));

            c.lineWidth = 6;
            c.strokeStyle = '#ff0000';
            c.stroke();

            points.forEach( function(e,i){
                c.beginPath();
                c.moveTo(25,e.y);
                c.lineTo(e.x,e.y);
                c.lineWidth = 1;
                c.strokeStyle = '#888';
                c.stroke();
                c.fillText('%1'.arg(e.v), 4, e.y+vtos)




            })

            points.forEach( function(e,i){
                c.beginPath();
                c.arc(e.x, e.y, 20, 0, Math.PI*2, false);
                c.fillStyle = '#ff0000';
                c.fill();

                c.beginPath();
                c.arc(e.x, e.y, 17, 0, Math.PI*2, false);
                c.fillStyle = '#ffffff';
                c.fill();

                c.font = '25px Roboto'
                c.textAlign="center";
                c.fillStyle = '#000000';
                c.fillText('%1'.arg(tr1[i]), e.x, e.y+vtos+4)
                c.textAlign="start";

                c.fillText('%1 В'.arg(e.v), e.x+15, e.y-15)
                //                c.fillText('%1 А'.arg(eI1[i]), e.x+15, e.y-15)
                c.font = '14pt Roboto'
            })



            c.textAlign="center";
            c.fillText('Участок 5602', width * 0.5, height -10 )
            c.textAlign="start";


            c.fillStyle = '#f00';
            c.globalAlpha=0.1;
            c.fillRect(30.5,v510,width-60,height-30-v510)
            c.fillStyle = '#000';

            c.textAlign="center";
            c.font = '34px Roboto'
            c.globalAlpha=0.4;
            c.fillText('Уровень недопустимого падения напряжения', width * 0.5, height-10 -  220)
            c.globalAlpha=1;
            c.textAlign="start";
            c.font = '14pt Roboto'

        }


        function getI( Ia, i ) {
            var ret = 0;
            for( var j = i; j < Ia.length; ++j )
                ret += Ia[j]
            return ret;
        }

        function gx( p, l ) {
            return 30.5 + p * (width - 60);
        }
    }

    Canvas {
        id: c2
        anchors.fill: parent
        onPaint: {
            var c = getContext('2d')
            var points = []
            c.strokeStyle = '#000000'
            c.clearRect(0, 0, width, height);



            c.textAlign="center";
            c.font = '34px Roboto'
            c.globalAlpha=0.3;
            c.fillText('Нормальный режим', width * 0.5, 70)
            c.globalAlpha=1;
            c.textAlign="start";
            c.font = '14pt Roboto'



            c.beginPath();
            c.moveTo(30.5,height);
            c.lineTo(30.5,30);
            c.lineWidth = 0.3;
            c.stroke();


            c.beginPath();
            c.moveTo(25.5, height - 30.5 );
            c.lineTo(width - 30, height - 30.5);
            c.lineWidth = 0.3;
            c.stroke();

            c.font = '14pt Roboto'
            c.fillText('U, В', 19, 17)


            var v400 = height - 30.5
            var v650 = 29.5
            var v600 = v400 - (v400 - v650) * 0.8
            var v420 = v400 - (v400 - v650) * 0.12
            var v510 = v400 - (v400 - v650) * 0.44

            var vtos = 4

            c.fillText('400', 4, v400 + vtos )

            //            c.beginPath();
            //            c.moveTo(25.5,v420);
            //            c.lineTo(width - 30,v420);
            //            c.lineWidth = 0.5;
            //            c.stroke();ff
            //            c.fillText('430', 4, v420+vtos)

            c.beginPath();
            c.moveTo(25.5,v600);
            c.lineTo(width - 30,v600);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('600', 4, v600+vtos)

            c.beginPath();
            c.moveTo(25.5,v650);
            c.lineTo(width - 30,v650);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('650', 4, v650+vtos)

            c.beginPath();
            c.moveTo(25.5,v510);
            c.lineTo(width - 30,v510);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('510', 4, v510+vtos)


            c.fillText('%1 м'.arg(l2), width - 65, height - 10 )


            var k =  0.93
            var y = v600 + ( height - 60 ) * (1 - k)
            c.beginPath();
            c.moveTo(30.5, y);
            for( var i = 0; i < ps2.length; ++i ) {
                var I = getI(eI1, i)

                var ll0 = i == 0 ? 0 : ps2[ i - 1 ]
                var ll1 = ps2[ i ]
                var dv = I*I*r*l2*(ll1-ll0)
                y += dv * (height-60)/250



                c.lineTo(gx(ps2[i],l2),y+ ( height - 60 ) * (1 - k));
                //                c.arc(gx(ps2[i],l1), y + ( height - 60 ) * (1 - k), 3, 0, Math.PI*2, false);
                var vv = -20+400 + 250 * (height - y-30) / (height - 60)
                points[ i ] = { 'x': gx(ps2[i],l2), 'y': y + ( height - 60 ) * (1 - k), 'v':~~(vv) }
            }

            c.lineTo(width - 30,y+ ( height - 60 ) * (1 - k));

            c.lineWidth = 6;
            c.strokeStyle = '#ff8800'
            c.stroke();

            points.forEach( function(e,i){
                c.beginPath();
                c.moveTo(25,e.y);
                c.lineTo(e.x,e.y);
                c.lineWidth = 1;
                c.strokeStyle = '#888';
                c.stroke();
                c.fillText('%1'.arg(e.v), 4, e.y+vtos)




            })

            points.forEach( function(e,i){
                c.beginPath();
                c.arc(e.x, e.y, 20, 0, Math.PI*2, false);
                c.fillStyle = '#ff8800';
                c.fill();

                c.beginPath();
                c.arc(e.x, e.y, 17, 0, Math.PI*2, false);
                c.fillStyle = '#ffffff';
                c.fill();

                c.font = '25px Roboto'
                c.textAlign="center";
                c.fillStyle = '#000000';
                c.fillText('%1'.arg(tr2[i]), e.x, e.y+vtos+4)
                c.textAlign="start";

                c.fillText('%1 В'.arg(e.v), e.x+15, e.y-15)
                c.font = '14pt Roboto'
            })

            c.textAlign="center";
            c.fillText('Участок 5605', width * 0.5, height -10 )
            c.textAlign="start";


            c.fillStyle = '#f00';
            c.globalAlpha=0.1;
            c.fillRect(30.5,v510,width-60,height-30-v510)
            c.fillStyle = '#000';

            c.textAlign="center";
            c.font = '34px Roboto'
            c.globalAlpha=0.4;
            c.fillText('Уровень недопустимого падения напряжения', width * 0.5, height-10 -  220)
            c.globalAlpha=1;
            c.textAlign="start";
            c.font = '14pt Roboto'
        }

        function getI( Ia, i ) {
            var ret = 0;
            for( var j = i; j < Ia.length; ++j )
                ret += Ia[j]
            return ret;
        }

        function gx( p, l ) {
            return 30.5 + p * (width - 60);
        }
    }

    Canvas {
        id: c3
        anchors.fill: parent
        onPaint: {
            var c = getContext('2d')
            var points = []
            c.strokeStyle = '#000000'
            c.clearRect(0, 0, width, height);


            c.textAlign="center";
            c.font = '34px Roboto'
            c.globalAlpha=0.3;
            c.fillText('Вынужденный режим', width * 0.5, 70)
            c.globalAlpha=1;
            c.textAlign="start";
            c.font = '14pt Roboto'



            c.beginPath();
            c.moveTo(30.5,height);
            c.lineTo(30.5,30);
            c.lineWidth = 0.3;
            c.stroke();


            c.beginPath();
            c.moveTo(25.5, height - 30.5 );
            c.lineTo(width - 30, height - 30.5);
            c.lineWidth = 0.3;
            c.stroke();

            c.font = '14pt Roboto'
            c.fillText('U, В', 19, 17)


            var v400 = height - 30.5
            var v650 = 29.5
            var v600 = v400 - (v400 - v650) * 0.8
            var v420 = v400 - (v400 - v650) * 0.12
            var v510 = v400 - (v400 - v650) * 0.44

            var vtos = 4

            c.fillText('400', 4, v400 + vtos )

            c.beginPath();
            c.moveTo(25.5,v420);
            c.lineTo(width - 30,v420);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('430', 4, v420+vtos)

            c.beginPath();
            c.moveTo(25.5,v600);
            c.lineTo(width - 30,v600);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('600', 4, v600+vtos)

            c.beginPath();
            c.moveTo(25.5,v650);
            c.lineTo(width - 30,v650);
            c.lineWidth = 0.5;
            c.stroke();
            c.fillText('650', 4, v650+vtos)

            //            c.beginPath();
            //            c.moveTo(25.5,v510);
            //            c.lineTo(width - 30,v510);
            //            c.lineWidth = 0.5;
            //            c.stroke();
            //            c.fillText('510', 4, v510+vtos)


            var xx = l1/l3 * (width - 60)
            c.beginPath();
            c.moveTo(xx,0);
            c.lineTo(xx,height);
            c.lineWidth = 1;
            c.strokeStyle = '#888'
            c.stroke();
            c.fillText('', xx-5, height-10)

            c.fillText('%1 м'.arg(l3), width - 65, height - 10 )



            var k = 0.93
            var y = v600 + ( height - 60  ) * (1 - k)
            c.beginPath();
            c.moveTo(30.5, y);
            var pp;
            var b = false
            for( var i = 0; i < ps3.length; ++i ) {
                var I = getI(eI3, i)
                var ll0 = i == 0 ? 0 : ps3[ i - 1 ]
                var ll1 = ps3[ i ]
                var dv = I*I*r*l3*(ll1-ll0)

                y += dv * (height-60)/250
                console.log(i, (ll1-ll0)*l3*I*I, I*I, I, ps3[i]*l3 )


                if( gx(ps3[i],l3) > l1/l3 * (width - 60) && !b ){
                    console.log( 'AAAA', i )
                    var p = getPointOfIntersection(
                                Qt.point(xx,0),
                                Qt.point(xx,height),
                                pp,
                                Qt.point(gx(ps3[i],l3), y + ( height - 60 ) * (1 - k))
                                )
                    c.lineTo(p.x, p.y)
                    c.lineWidth = 6
                    c.strokeStyle = '#ff0000'
                    c.stroke();
                    c.beginPath()
                    //moveto
                    c.moveTo(p.x, p.y)
                    b= true

                }
                pp =Qt.point(gx(ps3[i],l3), y + ( height - 60 ) * (1 - k))


                c.lineTo(gx(ps3[i],l3), y + ( height - 60 ) * (1 - k));
                //                c.arc(gx(ps3[i],l1), y + ( height - 60 ) * (1 - k), 3, 0, Math.PI*2, false);
                var vv = -20+400 + 250 * (height - y-30) / (height - 60)
                points[ i ] = { 'x': gx(ps3[i],l3), 'y': y + ( height - 60 ) * (1 - k), 'v':~~(vv) }
            }

            c.lineTo(width - 30,y+ ( height - 60 ) * (1 - k));

            c.lineWidth = 6;
            c.strokeStyle = '#ff8800'
            c.stroke();


            points.forEach( function(e, i){
                c.beginPath();
                c.moveTo(25,e.y);
                c.lineTo(e.x,e.y);
                c.lineWidth = 1;
                c.strokeStyle = '#888';
                c.stroke();

                c.fillText('%1'.arg(e.v), 4, e.y+vtos + ((i === points.length-1) ? 4 : 0))




            })

            points.forEach( function(e, i){
                c.beginPath();
                c.arc(e.x, e.y, 20, 0, Math.PI*2, false);
                c.fillStyle = i < 3 ? '#ff0000' : '#ff8800';
                c.fill();

                c.beginPath();
                c.arc(e.x, e.y, 17, 0, Math.PI*2, false);
                c.fillStyle = '#ffffff';
                c.fill();

                c.font = '25px Roboto'
                c.textAlign="center";
                c.fillStyle = '#000000';
                c.fillText('%1'.arg(tr3[i]), e.x, e.y+vtos+4)
                c.textAlign="start";

                c.fillText('%1 В'.arg(e.v), e.x+15, e.y-15)
                c.font = '14pt Roboto'
            })

            c.textAlign="center";
            c.fillText('Участок 5602', width * 0.3, height -10 )
            c.fillText('Участок 5605', width * 0.8, height -10 )
            c.textAlign="start";



            c.fillStyle = '#f00';
            c.globalAlpha=0.1;
            c.fillRect(30.5,v420,width-60,height-30-v420)
            c.fillStyle = '#000';

            c.textAlign="center";
            c.font = '34px Roboto'
            c.globalAlpha=0.4;
            c.fillText('Уровень недопустимого падения напряжения', width * 0.5, height-10 -  65)
            c.globalAlpha=1;
            c.textAlign="start";
            c.font = '14pt Roboto'
        }

        function getPointOfIntersection(p1, p2, p3, p4)
        {
            var d = (p1.x - p2.x) * (p4.y - p3.y) - (p1.y - p2.y) * (p4.x - p3.x);
            var da = (p1.x - p3.x) * (p4.y - p3.y) - (p1.y - p3.y) * (p4.x - p3.x);
            var db = (p1.x - p2.x) * (p1.y - p3.y) - (p1.y - p2.y) * (p1.x - p3.x);

            var ta = da / d;
            var tb = db / d;

            var dx = p1.x + ta * (p2.x - p1.x);
            var dy = p1.y + ta * (p2.y - p1.y);

            return Qt.point(dx, dy);
        }

        function getI( Ia, i ) {
            var ret = 0;
            for( var j = i; j < Ia.length; ++j )
                ret += Ia[j]
            return ret;
        }

        function gx( p, l ) {
            return 30.5 + p * (width - 60);
        }
    }


    /*
    SceneModel {
        id: m_sceneBuilder
        scene: m_scene.qGraphicsScene
    }
    Creatist {
        id: creatist
    }
    BspObject {
        id: bsp
        rect: m_scene.sceneRect
        maxDepth: 18
        Component.onCompleted: {
            console.log( 'bsp ready', rect, maxDepth )
        }
    }

    SplitView {
        anchors.fill: parent
        Rectangle {
            id: m_mainView
            width: root.width - 200
            color: '#333'
            DrawArea {
                id: drawarea
                backgroundColor: "transparent"
                anchors.fill: parent
                scene.width: m_scene.sceneRect.width
                scene.height: m_scene.sceneRect.height
                maxScale: 512
                property var dat: []
                property rect sourceRect: Qt.rect(scrollArea.contentX / scene.scale, scrollArea.contentY / scene.scale, width / scene.scale, height / scene.scale)
                grid.visible: false

                MouseArea {
                    id: scene_mouse
                    acceptedButtons: Qt.AllButtons
                    anchors.fill: parent
                }

                Scene {
                    id: m_scene
                    enableBsp: true
                    viewport: drawarea
                    width: sceneRect.width
                    height: sceneRect.height
                    sceneRect: Qt.rect(0, 0, 16000, 16000)
                    renderer.renderTarget: SceneRenderer.FramebufferObject
                    renderer.z: 1000
                    onViewTransformChanged: {
                        if( smoother.currentText != null ) {
                            bsp.clear()
                            bsp.insertPoints(smoother.currentText.mesh, 0)
                            var rects = bsp.dumpRects()
                            bspRectsModel.clear()
                            rects.forEach(function(e){
                                bspRectsModel.append( {
                                                         px: e.x,
                                                         py: e.y,
                                                         pw: e.width,
                                                         ph: e.height
                                                     })
                            })
                        }
                    }



                }



                Repeater {
                    model: bspRectsModel
                    Rectangle {
                        x: px
                        y: py
                        width: pw
                        height: ph
                        color: '#fff'
                        opacity: 1/7.0
                    }
                }


            }
//            TextLayer {
//                id: textLayer
//                anchors.fill: parent
//                font.family: 'Roboto'
//                color: '#fff'
//                viewTransform: m_scene.viewTransform
//                viewportRect: m_scene.viewportRect
//            }
        }

        Rectangle{
            id: m_sideBar
            color: '#bbb'
            ColumnLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                CheckBox {
                    id: drawpol
                    text: 'draw polygon'
                }

                Button {
                    Layout.fillWidth: true
                    text: 'add predefined polygon'
                    onClicked: {
                        var phandle = m_sceneBuilder.addItem("PolygonItem", { flags:
                                                                 VisualInstance.ItemIsSelectable |
                                                                 VisualInstance.ItemIsMovable |
                                                                 VisualInstance.ItemSendsGeometryChanges
                                                             })
                        //                                phandle.polygon = smoother.output
                        phandle.pen.color = smoother.currentItem.pen.color
                        phandle.pen.cosmetic = true
                        phandle.pen.width = 2
                        phandle.shapeType = 1
                        phandle.hoverEnabled = true
                        phandle.onMousePress = onClick
                        phandle.onHoverEnter = onHoverEnter
                        phandle.onHoverLeave = onHoverLeave
                        phandle.bind(VisualInstance.ItemPositionChange, itemPosChange)
                        polmodel.clear()
                        polfactory.buildPolygon()

                        //                                drawpol.checked = false
                    }
                    function onClick(event) {
                        console.log("polygon pressed", this)
                    }
                    property var oldColor
                    onOldColorChanged: console.log("old color", oldColor)
                    function onHoverEnter(event) {
                        //                                console.log("hover enter", this)
                        oldColor = this.pen.color
                        this.pen.color = "#FFFFFF"
                        this.update()
                    }
                    function onHoverLeave(event) {
                        //                                console.log("hover leave", this)
                        //                                oldColor = this.pen.color
                        this.pen.color = oldColor
                        this.update()
                    }
                    function itemPosChange(value) {
                        value.x = Math.round(value.x / 8) * 8
                        value.y = Math.round(value.y / 8) * 8
                        return value.point
                    }
                }

                Button {
                    id: checkButton
                    text: 'check BSP rects'
                    onClicked: {
                        var rects = bsp.dumpRects()
                        bspRectsModel.clear()
                        rects.forEach(function(e){
                            bspRectsModel.append( {
                                                     px: e.x,
                                                     py: e.y,
                                                     pw: e.width,
                                                     ph: e.height
                                                 })
                        })
                    }
                }
            }
        }
    }


    /*****************************************
    ListModel {
        id: bspRectsModel
    }


    /*****************************************/


    /*****************************************
    Item {
        id: polygonDrawingStuff
        states: [
            State {
                name: "createpoly"
                when: drawpol.checked
                PropertyChanges {
                    target: scene_mouse
                    onClicked: {
//                        bsp.insertPoint( Qt.point( mouse.x, mouse.y ), mouse.x )
                        polmodel.append({ xp: mouse.x, yp: mouse.y })
                        polfactory.buildPolygon()
                    }
                }
            }
        ]

        ListModel {
            id: polmodel
        }

        QtObject {
            id: smoother
            property var currentItem
            property var currentText
            property var currentDebugItem
//            property alias smooth: m_sidebar.curveSmoothing
//            property alias precision: m_sidebar.curvePrecision
//            property alias offset: m_sidebar.baseCurveOffset
//            property alias fast: m_sidebar.fastMode

            property int smooth: 30 //m_sidebar.curveSmoothing
            property int precision: 5 //m_sidebar.curvePrecision
            property int offset: 0 //m_sidebar.baseCurveOffset
            property bool fast: false //m_sidebar.fastMode

            property var polygon: polfactory.polygon

            onPolygonChanged: {
                if (!m_sceneBuilder.scene)
                    return
                if (!currentItem) {

                    currentItem = m_sceneBuilder.addItem("GuideStroke",
                                                         {
                                                             smooth: smooth,
                                                             offset: offset,
                                                             precision: precision,
                                                             fast: fast
                                                         })


//                    currentText = m_sceneBuilder.addItem("GuideText",
//                                                         {
//                                                             text: 'SAMPLE TEXT !!!!',
//                                                             font: 'Arial',
//                                                             textColor: '#ffffff',
//                                                             orientation: Qt.Vertical
//                                                         })


                    currentItem.pen.cosmetic = true
                    currentItem.pen.color = Qt.hsla(Math.random(), 0.8, 0.8)
                    currentItem.pen.width = 2
                    currentItem.pen.strokeStyle = Qt.DotLine
                    currentItem.pen.cosmetic = true
                    currentItem.sourcePen.width = 1
                    currentItem.sourcePen.color = "#00ff80"
                    currentItem.sourcePen.cosmetic = true
                    currentItem.drawSource = true
                    currentItem.setDebugMode(true)

                }

                var g = currentItem.source
                g.setPolygon(polygon)
                currentItem.source = g

                if( currentItem.result.length > 200 )
                    textLayer.appendLabel(currentItem.result, 'RANDOM STATIC TEXT', 20);


//                currentText.path = currentItem.result
//                console.log( 'text changed' )
            }

            onSmoothChanged: updateCurrentItem()
            onOffsetChanged: updateCurrentItem()
            onPrecisionChanged: updateCurrentItem()
            onFastChanged: updateCurrentItem()

            function updateCurrentItem() {
                currentItem.smooth = smooth
                currentItem.precision = precision
                currentItem.offset = offset
                currentItem.fast = fast
                var r = currentItem.source
            }
        }

        PolygonFactory {
            id: polfactory
            model: polmodel
            xRole: "xp"
            yRole: "yp"
            property var currentItem
            onPolygonChanged:  {
                if (!currentItem) {
                    currentItem = m_sceneBuilder.addItem("PolygonItem", { polygon: polygon })
                    currentItem.pen.cosmetic = true
                    currentItem.pen.width = 1
                    currentItem.shapeType = 1
                    currentItem.pen.color = "#80ffffff"
                    currentItem.pen.strokeStyle = Qt.DashLine




                }
                currentItem.polygon = polygon


            }
        }

    }
    /*****************************************/
}

