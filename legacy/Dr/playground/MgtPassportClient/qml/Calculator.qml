import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Window {
    id: root
    title: 'ЦДП. 2015. Расчёт токов при одностороннем питании КС. ' + ksname
    property string ksname: ''
    property alias siteLength: lusc.value
    property alias k1pl: l1_2.value
    property alias k1ps: l1_4.value
    property alias k1pc: mk1_0.checked
    property alias k1ml: l1_3.value
    property alias k1ms: l1_5.value
    property alias k1mc: mk1_1.checked
    property alias istram: o1.checked

onVisibleChanged: if( !visible ) calc.reset()
//    property alias k1pl:

    width: contentLayout.implicitWidth + 10
    height: contentLayout.implicitHeight + 10

    property real scaleX: scaleWidth
    property real scaleY: scaleHeight
    property real scaleWidth: 21480 / width
    property real scaleHeight: 13095 / height

    QtObject{
        id: calc

        property var f1: [f1_0,f1_1,f1_2,f1_3]// TextField
        property var nm1: [nm1_0,nm1_1,nm1_2,nm1_3]// TextField
        property var l1: [null,null,l1_2,l1_3,l1_4,l1_5,l1_6,l1_7,l1_8,l1_9]// TextField

        property var mk1: [mk1_0,mk1_1,mk1_2,mk1_3]// CheckBox
        property var nd1: [nd1_0,nd1_1,nd1_2,nd1_3]// CheckBox
        property var tpm1: [tpm1_0,tpm1_1,tpm1_2,tpm1_3]// CheckBox

        property var m1: [m1_0,m1_1,m1_2,m1_3,m1_4,m1_5,m1_6,m1_7]// Label
        property var m10: [m10_0]// Label
        property var n1: [n1_0,n1_1,n1_2,n1_3,n1_4,n1_5,n1_6,n1_7,n1_8,n1_9,n1_10,n1_11,n1_12,n1_13]// Label
        property var n221: [n221_0,n221_1,n221_2]// Label
        property var n444: [n444_0,n444_1]// Label
        property var n9: [n9_0]// Label
        property var os1: [os1_0,os1_1,os1_2,os1_3,os1_4]// Label

        function fNag_Click() {
            reset()
            var nm2 = []
            var o = []
            for( var e = 0; e <= 4; ++e )
                os1[e].visible = false

            var oo = 2
            if(!o1.checked) {
                if(!o2.checked) {
                    o2.checked = true
                }
            }
            var itr, v, b, ki, c, a
            if(o1.checked) {
                if(leto.checked) {
                    itr = 85
                    v = 15
                    b = 1.7
                    ki = 0.85
                    c = 1000
                    a = 2
                }else{
                    itr = 105
                    v = 15
                    b = 1.7
                    ki = 0.85
                    c = 1000
                    a = 2
                }
            }
            if(o2.checked) {
                if(leto.checked) {
                    itr = 74
                    v = 17
                    b = 0.8
                    ki = 0.92
                    c = 700
                    a = 1.7
                }else{
                    itr = 92
                    v = 17
                    b = 0.8
                    ki = 0.92
                    c = 700
                    a = 1.7
                }
            }
            // выбор сечения ***************
            var skp1, skp2
            if(skc.checked) {
                if(o1.checked) {
                    if(sd.checked) {
                        skp1 = 200
                        skp2 = 2500
                    }else{
                        skp1 = 100
                        skp2 = 2500
                    }
                }else{
                    skp1 = 100
                    skp2 = 100
                }
            }else{
                if(o1.checked) {
                    if(sd.checked) {
                        skp1 = 170
                        skp2 = 2500
                    }else{
                        skp1 = 85
                        skp2 = 2500
                    }
                }else{
                    skp1 = 85
                    skp2 = 85
                }
            }
            // ***********************
            if(lusc.value === 0) {
                oo = 1
                os1[4].visible = true
                os1[1].visible = true
            }
            if(l1[2].value === 0 ) {
                oo = 1
                os1[2].visible = true
                os1[4].visible = true
            }
            if(l1[3].value === 0 ) {
                oo = 1
                os1[3].visible = true
                os1[4].visible = true
            }
            var i23 = 0
            var enm = 0
            var ooo, nmm, ke, iplot2, udrCu, padUkc2, padUkc3, padUkc4, udrk1, udrk2, udrk3, udrk4, w, lpolkab2, sdplkab, lotrkab2, sdotrkab, s3plkab, s4plkab, rkpol1, rkpol, rkpol3, padUkabpol2, rkotr2, rkotr, rkotr4, padUkabotr2, rr, kol, cumpadU2, sumpadU2, rkc, rkc2, rkcL1, rkcL2, rkc4, nnom1, i2nom, iplot2nom, nmax2, i2max, iplot2max

            if(oo  ==  2) {
                //***** новое ************************
                for( var m = 0; m <= 3; ++m ) {
                    if(f1[m].value === 0) {
                        o[m] = 3
                        if(nm1[m].value === 0) {
                            o[m] = 3
                        }else{
                            o[m] = 2
                        }
                    }else{
                        o[m] = 4
                        nm1[m].value = 0
                    }
                    //***************
                    ooo = o[0]
                    //******** расчёт на базе N - частоты движения
                    if(o[m]  ==  4) {
                        nmm = (f1[m].value * lusc.value / (v * 1000)) * 1000
                        nm1[m].value = ( ~~( nmm ) ) / 1000.0
                        if(tpm1[m].checked) {

                            if(o1.checked) { // задание тока машины
                                nm2[m] = nm1[m].value * 2
                            }else{
                                nm2[m] = nm1[m].value * 1.4
                            }

                            if(nd1[m].checked) {
                                i23 = i23 + itr * nm2[m]
                                enm = enm + nm2[m]
                            }else{
                                i23 = i23 + 2 * itr * nm2[m]
                                enm = enm + 2 * nm2[m]
                            }
                        }else{
                            nm2[m] = nm1[m].value
                            if(nd1[m].checked) {
                                i23 = i23 + itr * nm2[m]
                                enm = enm + nm2[m]
                            }else{
                                i23 = i23 + 2 * itr * nm2[m]
                                enm = enm + 2 * nm2[m]
                            }
                        }

                        //******** Расчёт на базе числа машин на участке
                    }else{
                        if(o[m] ==  2) {
                            if(tpm1[m].checked) {

                                if(o1.checked) { // задание тока машины
                                    nm2[m] = nm1[m].value * 2
                                }else{
                                    nm2[m] = nm1[m].value * 1.4
                                }

                                if(nd1[m].checked) {
                                    i23 = i23 + itr * nm2[m]
                                    enm = enm + nm2[m]
                                }else{
                                    i23 = i23 + 2 * itr * nm2[m]
                                    enm = enm + 2 * nm2[m]
                                }
                            }else{
                                nm2[m] = nm1[m].value
                                if(nd1[m].checked) {
                                    i23 = i23 + itr * nm2[m]
                                    enm = enm + nm2[m]
                                }else{
                                    i23 = i23 + 2 * itr * nm2[m]
                                    enm = enm + 2 * nm2[m]
                                }
                            }
                        }
                    }
                }
                enm1.value = ( ~~( enm ) )
                // проверка enm
                if(enm != 0) {

                    //************************************
                    //i2 = itr * enm //ток на участке КС в одном направлении
                    i.value = ( ~~( i23 ) )
                    ke = Math.pow((1 + b / enm), 0.5)
                    iplot2 = i23 * ke / (ki * (skp1 + skp2)) // сечение + и - (рельса)
                    iplot.value = ( ~~( iplot2 * 100 ) ) / 100

                    iustLB.value = ( ~~( 2 * i23 + c ) )

                    //падение напряжения на положит. КС при двух паралл. проводах  (рельсах), (движение в оба конца)
                    udrCu = 1.8 * Math.pow( 10, -8 )
                    // ** положительный провод КС
                    padUkc3 = (i23 * lusc.value * udrCu * (1 + (3 * a - 2) / (2 * enm))) / (3 * ki * 2 * skp1 * Math.pow(10 ,-6))
                    // ** отриц. провод КС (рельс)
                    padUkc4 = (i23 * lusc.value * udrCu * (1 + (3 * a - 2) / (2 * enm))) / (3 * ki * 2 * skp2 * Math.pow(10 ,-6))
                    padUkc2 = (padUkc3 + padUkc4) / 2
                    padUkc.value = (( ~~( padUkc2 * 1000 ) )) / 1000
                    //падение напряжения на кабеле

                    if(mk1[0].checked) {
                        udrk1 = 1.8 * Math.pow(10 ,-8)
                    }else{
                        udrk1 = 3.03 * Math.pow(10 ,-8)
                    }

                    if(mk1[1].checked) {
                        udrk2 = 1.8 * Math.pow(10 ,-8)
                    }else{
                        udrk2 = 3.03 * Math.pow(10 ,-8)
                    }

                    if(l1[6].value === 0 ) {
                        w = 1
                    }else{
                        lpolkab2 = l1[6].value * Math.pow(10 ,-6)
                        if( l1[7].value === 0 ) {
                            sdplkab = 800 * Math.pow(10 ,-6)
                        }else{
                            sdplkab = l1[7].value * Math.pow(10 ,-6)
                        }
                        if(mk1[2].checked) {
                            udrk3 = 1.8 * Math.pow(10 ,-8)
                        }else{
                            udrk3 = 3.03 * Math.pow(10 ,-8)
                        }
                    }

                    if(l1[8].value === 0 ) {
                        w = 1
                    }else{
                        lotrkab2 = l1[8].value * Math.pow(10 ,-6)
                        if(l1[9].value === 0) {
                            sdotrkab = 800 * Math.pow(10 ,-6)
                        }else{
                            sdotrkab = l1[9].value * Math.pow(10 ,-6)
                        }
                        if(mk1[3].checked) {
                            udrk4 = 1.8 * Math.pow(10 ,-8)
                        }else{
                            udrk4 = 3.03 * Math.pow(10 ,-8)
                        }
                    }


                    // **Задание сечения кабелей
                    if(l1[4].value === 0) {
                        s3plkab = 800 * Math.pow(10 ,-6)
                    }else{
                        s3plkab = l1[4].value * Math.pow(10 ,-6)
                    }

                    if(l1[5].value === 0 ) {
                        s4plkab = 800 * Math.pow(10 ,-6)
                    }else{
                        s4plkab = l1[5].value * Math.pow(10 ,-6)
                    }
                    // ********** rkpol - сопротивл. полож. каб.
                    if(l1[6].value === 0 ) {
                        rkpol1 = l1[2].value * udrk1 / s3plkab
                        rkpol = rkpol1
                    }else{
                        rkpol1 = l1[2].value * udrk1 / s3plkab
                        rkpol3 = l1[6].value * udrk3 / sdplkab
                        rkpol = rkpol1 * rkpol3 / (rkpol1 + rkpol3)
                    }
                    padUkabpol2 = Number(i.value) * rkpol * (1 + (a - 1) / enm)
                    padUkabpol.value = (( ~~( padUkabpol2 * 1000 ) )) / 1000
                    // ********** rkpol - сопротивл. отриц. каб.

                    if(l1[8].value === 0 ) {
                        rkotr2 = l1[3].value * udrk2 / s4plkab
                        rkotr = rkotr2
                    }else{
                        rkotr2 = l1[3].value * udrk2 / s4plkab
                        rkotr4 = l1[8].value * udrk4 / sdotrkab
                        rkotr = rkotr4 * rkotr2 / (rkotr4 + rkotr2)
                    }

                    padUkabotr2 = Number(i.text) * rkotr * (1 + (a - 1) / enm)
                    padUkabotr.value = (( ~~( padUkabotr2 * 1000 ) )) / 1000
                    //***************************************
                    rr = lusc.value * udrCu / (skp1 * Math.pow(10 ,-6))
                    kol = 2 // Кол-во пар проводов КС
                    cumpadU2 = padUkc3 + padUkc4 + padUkabotr2 + padUkabpol2
                    cumpadU.value = (( ~~( cumpadU2 * 1000 ) )) / 1000
                    //************************
                    sumpadU2 = 1.5 * (padUkc3 + padUkc4) + padUkabotr2 + padUkabpol2
                    sumpad.value = (( ~~( sumpadU2 * 1000 ) )) / 1000
                    //********** ! rkc - сопротивление контактного провода одной полярности в одном направлени
                    rkc = lusc.value * udrCu / (ki * skp1 * Math.pow(10 ,-6))
                    // ********* ! rkc2 - сопротивление конт.  провода одной полярности в обоиx направлениях
                    rkc2 = lusc.value * udrCu / (ki * 2 * skp1 * Math.pow(10 ,-6))
                    // сопротвление рельса
                    rkcL1 = lusc.value * udrCu / (ki * skp2 * Math.pow(10 ,-6))
                    rkcL2 = lusc.value * udrCu / (ki * 2 * skp2 * Math.pow(10 ,-6))
                    kol = 2 // Кол-во пар проводов КС
                    rkc4 = rkc2 + rkcL2 // суммарное сопротивление - провода + рельс

                    nnom2.value = (90 / itr - 1 / 6 * rkc4 * (3 * a - 2) - (a - 1) * (rkpol + rkotr)) / (1 / 3 * rkc4 + rkpol + rkotr)




                    nnom1 = nnom2.value * v * 1000 / (2 * lusc.value)
                    nnom.value = ( ~~( nnom1 ) )
                    nnom2.value = ( ~~( nnom2.value ) )

                    i2nom = 2 * itr * nnom.value * lusc.value / (v * 1000)
                    inom.value = ( ~~( i2nom ) )
                    if(nnom.value === 0 ) {
                        ke = 1
                    }else{
                        ke =Math.pow( (1 + b / nnom.value) , 0.5)
                    }
                    iplot2nom = i2nom * ke / (ki * (skp1 + skp2))
                    iplotnom.value = ( ~~( iplot2nom * 100 ) ) / 100
                    //*************************************
                    nmax1.value = (160 / itr - 1.5 / 6 * rkc4 * (3 * a - 2) - (a - 1) * (rkpol + rkotr)) / (1.5 / 3 * rkc4 + rkpol + rkotr)
                    nmax2 = nmax1.value * v * 1000 / (2 * lusc.value)
                    nmax.value = ( ~~( nmax2 ) )

                    i2max = 2 * itr * nmax.value * lusc.value / (v * 1000)
                    imax.value = ( ~~( i2max ) )
                    if(nnom.value ===  0) {
                        ke = 1
                    }else{
                        ke = Math.pow((1 + b / nmax.value) , 0.5)
                    }

                    iplot2max = i2max * ke / (ki * (skp1 + skp2))
                    iplotmax.value = ( ~~( iplot2max * 100 ) ) / 100
                } // от проверки enm
            }
        }

        function fo1_Click() {
            for( var n = 0; n <= 3; ++n ) {
                tpm1[n].text = "Сдвоен."
            }
        }

        function fo2_Click() {
            for( var n = 0; n <= 3; ++n ) {
                tpm1[n].text = "ЗИУ-10"
            }
        }

        function fr1_Click() {
            //           Dim r1 As Variant
            //           Dim r2 As Variant
            //           Dim r3 As Variant
            //           Dim r4 As Variant
            //           Dim ir As String

            var r1
            var r2
            var r3
            var r4
            var ir

            var q = 1
            var ll1 = 1
            var ll2 = 1
            var ll3 = 1
            var ll4 = 1
            var l11 = lusc.value
            var l2 = lusc.value
            var l3 = l1[2].value
            var l4 = l1[3].value
            var l5 = l1[6].value
            var l6 = l1[8].value
            var ll1 = l11
            var ll2 = l2
            var ll31 = l3
            var ll41 = l4
            var ll32 = l5
            var ll42 = l6
            irez.value = 0
            var udrAL = 3.03 * Math.pow(10 ,-8)
            var udrCu = 1.8 * Math.pow(10 ,-8)
            // ** Сечение провода КС


            var s1plkc, s2mikc, s31plkab, s41mikab
            if(skc.checked) {
                if(o1.checked) {
                    if(sd.checked) {
                        s1plkc = 200 * Math.pow(10 ,-6)
                        s2mikc = 2500 * Math.pow(10 ,-6)
                    }else{
                        s1plkc = 100 * Math.pow(10 ,-6)
                        s2mikc = 2500 * Math.pow(10 ,-6)
                    }
                }else{
                    s1plkc = 100 * Math.pow(10 ,-6)
                    s2mikc = 100 * Math.pow(10 ,-6)
                }
            }else{
                if(o1.checked) {
                    if(sd.checked) {
                        s1plkc = 170 * Math.pow(10 ,-6)
                        s2mikc = 2500 * Math.pow(10 ,-6)
                    }else{
                        s1plkc = 85 * Math.pow(10 ,-6)
                        s2mikc = 2500 * Math.pow(10 ,-6)
                    }
                }else{
                    s1plkc = 85 * Math.pow(10 ,-6)
                    s2mikc = 85 * Math.pow(10 ,-6)
                }
            }
            //s3plkab = 800 * Math.pow(10 ,-6)
            //s4mikab = 800 * Math.pow(10 ,-6)

            // **Задание сечения кабелей
            if(l1[4].value === 0) {
                s31plkab = 800 * Math.pow(10 ,-6)
            }else{
                s31plkab = l1[4].value *Math.pow(10 ,-6)
            }
            if(l1[5].value === 0) {
                s41mikab = 800 * Math.pow(10 ,-6)
            }else{
                s41mikab = l1[5].value * Math.pow(10 ,-6)
            }



            var u = 600
            r1 = udrCu * ll1 / (s1plkc * 2)
            r2 = udrCu * ll2 / (s2mikc * 2)
            var r31, r41, qq
            if(mk1[0].checked) { // выбор материала кабеля
                r31 = udrCu * ll31 / s31plkab
            }else{
                r31 = udrAL * ll31 / s31plkab
            }

            if(mk1[1].checked) { // выбор материала кабеля
                r41 = udrCu * ll41 / s41mikab
            }else{
                r41 = udrAL * ll41 / s41mikab
            }

            if(l1[6].value === 0) {
                ll32 = 0
                if(l1[8].value === 0 ) {
                    ll42 = 0
                    qq = 2 //нет вторых кабелей
                }else{
                    qq = 3 // есть только 2-й минус
                }
            }else{
                if(l1[8].value === 0 ) {
                    qq = 4 // есть только 2-й плюс
                }else{
                    qq = 5 // усть два вторых кабеля
                }
            }





            if(qq  ==  2) {
                ir = u / (r1 + r2 + r31 + r41)
            }
            var s32plkab, r32, s42mikab, r42

            if(qq  ==  4) {
                if(l1[7].value === 0) {
                    s32plkab = 800 * Math.pow(10 ,-6)
                }else{
                    s32plkab = l1[7].value * Math.pow(10 ,-6)
                }
                if(mk1[2].checked) { // выбор материала кабеля
                    r32 = udrCu * ll32 / s32plkab
                }else{
                    r32 = udrAL * ll32 / s32plkab
                }
                ir = u / (r1 + r2 + r41 + r31 * r32 / (r31 + r32))
            }

            if(qq  ==  5) {
                if(l1[9].value === 0) {
                    s42mikab = 800 * Math.pow(10 ,-6)
                }else{
                    s42mikab = l1[9].value * Math.pow(10 ,-6)
                }
                if(mk1[3].checked) { // выбор материала кабеля
                    r42 = udrCu * ll42 / s42mikab
                }else{
                    r42 = udrAL * ll42 / s42mikab
                }
                if(l1[7].value === 0) {
                    s32plkab = 800 * Math.pow(10 ,-6)
                }else{
                    s32plkab = l1[7].value * Math.pow(10 ,-6)
                }
                if(mk1[2].checked) { // выбор материала кабеля
                    r32 = udrCu * ll32 / s32plkab
                }else{
                    r32 = udrAL * ll32 / s32plkab
                }
                ir = u / (r1 + r2 + r31 * r32 / (r31 + r32) + r41 * r42 / (r41 + r42))
            }


            if(qq  ==  3) {
                if(l1[9].value === 0) {
                    s42mikab = 800 * Math.pow(10 ,-6)
                }else{
                    s42mikab = l1[9].value * Math.pow(10 ,-6)
                }
                if(mk1[3].checked) { // выбор материала кабеля
                    r42 = udrCu * ll42 / s42mikab
                }else{
                    r42 = udrAL * ll42 / s42mikab
                }
                ir = u / (r1 + r2 + r31 + r41 * r42 / (r41 + r42))
            }


            var ir2 = 1 + ir - 1
            irez.value = ( ~~( ir ) )
            //irez = ir
            var iustp = ( ~~( ir * 0.85 ) ) / 100
            iust.value = ( ~~( iustp ) ) * 100

        }

        function reset() {
//            f1.forEach( function(e){ e.text = '' })
            nm1.forEach( function(e){ e.value = 0 })
//            l1.forEach( function(e){ if( e!==null ) e.text = '' })
//            mk1.forEach( function(e){ e.checked = false })
            nd1.forEach( function(e){ e.checked = false })
            tpm1.forEach( function(e){ e.checked = false })
//            lusc.text = ''
            sd.checked = false
            skc.checked = false
            i.value = 0
            iplot.value = 0
            cumpadU.value = 0
            sumpad.value = 0
            iustLB.value = 0
            padUkc.value = 0
            padUkabpol.value = 0
            padUkabotr.value = 0
            nnom.value = 0
            nnom2.value = 0
            inom.value = 0
            iplotnom.value = 0
            nmax.value = 0
            nmax1.value = 0
            imax.value = 0
            iplotmax.value = 0
            irez.value = 0
            iust.value = 0
            enm1.value = 0
        }
    }
    GridLayout {
        id: contentLayout
        columns: 3
        GroupBox {
            title: 'Параметры кабелей и КС.'
            ColumnLayout {
                id: firstcolumn
                RowLayout { // KS length
                    Label { // n8
                        id: n8
                        text: 'Длина участка КС'
                    }
                    SpinBox { // lusc
                        minimumValue: 0
                        maximumValue: 100000
                        decimals: 2
                        id: lusc
                        suffix: ' м'
                    }
                    CheckBox { // sd
                        id: sd
                        text: 'Сдвоен.'
                    }
                    CheckBox { // skc
                        id: skc
                        text: '100 мм'
                    }
                }
                GroupBox {
                    GridLayout { // cabels
                        columns: 4
                        Label {}
                        Label { // n444
                            id: n444_0
                            text: 'Длина'
                            font.family: 'MS Sans Serif'
                            font.weight: Font.Bold
                            font.pointSize: 8.25
                        }
                        Label { // n444
                            id: n444_1
                            text: 'Сечение'
                            font.family: 'MS Sans Serif'
                            font.weight: Font.Bold
                            font.pointSize: 8.25
                        }
                        Label {
                            text: 'Марка'
                            font.family: 'MS Sans Serif'
                            font.weight: Font.Bold
                            font.pointSize: 8.25
                        }
                        Label { // n3
                            id: n3
                            text: 'Положительный кабель'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_2
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' м'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_4
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' мм'
                        }
                        CheckBox { // Mk1
                            id: mk1_0
                            text: 'Cu'
                        }
                        Label { // n9
                            id: n9_0
                            text: 'Отрицательный кабель'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_3
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' м'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_5
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' мм'
                        }
                        CheckBox { // Mk1å
                            id: mk1_1
                            text: 'Cu'
                        }
                        Label { // n4
                            id: n4
                            text: 'Положительный кабель № 2.'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_6
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' м'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_7
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' мм'
                        }
                        CheckBox { // Mk1
                            id: mk1_2
                            text: 'Cu'
                        }
                        Label { // m10
                            id: m10_0
                            text: 'Отрицательный кабель № 2.'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_8
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' м'
                        }
                        SpinBox {//TextField { // l1
                            id: l1_9
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' мм'
                        }
                        CheckBox { // Mk1
                            id: mk1_3
                            text: 'Cu'
                        }
                    }
                }

            }
        }
        GroupBox {
            id: nd2
            title: 'Параметры подвижного состава и его эксплуатации. '
            Layout.fillWidth: true
            Layout.fillHeight: true
            ColumnLayout {
//                implicitHeight: firstcolumn.implicitHeight
                anchors.fill: parent
                Layout.fillWidth: true
                GridLayout {
                    columns: 4
                    Layout.fillWidth: true
                    Label{Layout.fillWidth: true}
                    Label { // n7
                        id: n7
                        text: 'Частота движения, пар ваг./час'
                    }
                    Label{}
                    Label{}


                    Label { // m1
                        id: m1_4
                        Layout.fillWidth: true
                        text: 'Маршрут № 1'
                    }
                    SpinBox {//TextField { // f1
                        id: f1_0
                        implicitWidth: n7.width
                        minimumValue: 0
                        maximumValue: 100000
                        decimals: 2
                    }
                    CheckBox { // tpm1
                        id: tpm1_0
                        text: 'ЗИУ-10'
                    }
                    CheckBox { // nd1
                        id: nd1_0
                        text: 'В одном направлении'
                    }

                    Label { // m1
                        id: m1_5
                        Layout.fillWidth: true
                        text: 'Маршрут № 2'
                    }
                    SpinBox {//TextField { // f1
                        id: f1_1
                        implicitWidth: n7.width
                        minimumValue: 0
                        maximumValue: 100000
                        decimals: 2
                    }
                    CheckBox { // tpm1
                        id: tpm1_1
                        text: 'ЗИУ-10'
                    }
                    CheckBox { // nd1
                        id: nd1_1
                        text: 'В одном направлении'
                    }

                    Label { // m1
                        id: m1_6
                        Layout.fillWidth: true
                        text: 'Маршрут № 3'
                    }
                    SpinBox {//TextField { // f1
                        id: f1_2
                        implicitWidth: n7.width
                        minimumValue: 0
                        maximumValue: 100000
                        decimals: 2
                    }
                    CheckBox { // tpm1
                        id: tpm1_2
                        text: 'ЗИУ-10'
                    }
                    CheckBox { // nd1
                        id: nd1_2
                        text: 'В одном направлении'
                    }

                    Label { // m1
                        id: m1_7
                        Layout.fillWidth: true
                        text: 'Маршрут № 4'
                    }
                    SpinBox {//TextField { // f1
                        id: f1_3
                        implicitWidth: n7.width
                        minimumValue: 0
                        maximumValue: 100000
                        decimals: 2
                    }
                    CheckBox { // tpm1
                        id: tpm1_3
                        text: 'ЗИУ-10'
                    }
                    CheckBox { // nd1
                        id: nd1_3
                        text: 'В одном направлении'
                    }
                }
                GroupBox {
                    Layout.fillWidth: true

                    RowLayout{
                        Layout.fillWidth: true
                        RadioButton { // o2
                            ExclusiveGroup{id: exclusivegroupforcalc }
                            id: o2
                            exclusiveGroup: exclusivegroupforcalc
                            text: 'Троллейбус'
                            checked: true
                            onClicked: calc.fo2_Click()
                        }
                        RadioButton { // o1
                            id: o1
                            exclusiveGroup: exclusivegroupforcalc
                            text: 'Трамвай'
                            onClicked: calc.fo1_Click()
                        }
                        CheckBox { // leto
                            id: leto
                            text: 'Лето - Осень'
                        }
                        Item {Layout.fillWidth: true}
                    }
                }
            }
        }
        GroupBox {
            id: n222
            title: 'Кол-во машин i-го маршрута в одном направлении на участ-ке рассчёта, ед.'
            GridLayout {
                columns: 2
                implicitHeight: firstcolumn.implicitHeight
                Label { // m1
                    id: m1_0
                    text: 'Маршрут № 1'
                }
                SpinBox {//TextField { // nm1
                    id: nm1_0
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                }
                Label { // m1
                    id: m1_1
                    text: 'Маршрут № 2'
                }
                SpinBox {//TextField { // nm1
                    id: nm1_1
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                }
                Label { // m1
                    id: m1_2
                    text: 'Маршрут № 3'
                }
                SpinBox {//TextField { // nm1
                    id: nm1_2
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                }
                Label { // m1
                    id: m1_3
                    text: 'Маршрут № 4'
                }
                SpinBox {//TextField { // nm1
                    id: nm1_3
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                }
                Label { // n221
                    id: n221_0
                    text: 'Общее кол-во машин на\nучастке расчёта, приведённое к ЗИУ-9.'
                }
                SpinBox {//TextField { // enm1
                    id: enm1
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                }
                Item{Layout.fillHeight: true}
            }
        }

        GroupBox {
            id: nd4
            title: 'Результаты  рассчётов.'
            Layout.fillWidth: true
            Layout.fillHeight: true
            GridLayout {
                anchors.fill: parent
                columns: 2
                Label { // n1
                    id: n1_0
                    text: 'Ток участка'
                }
                SpinBox {//TextField { // i
                    id: i
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' A'
                }
                Label { // n1
                    id: n1_1
                    text: 'Плотность тока'
                }
                SpinBox {//TextField { // iplot
                    id: iplot
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' A/мм²'
                }
                Label { // n1
                    id: n1_2
                    text: 'Суммарное падение U'
                }
                SpinBox {//TextField { // CumpadU
                    id: cumpadU
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' B'
                }
                Label { // n1
                    id: n1_13
                    text: 'Суммарное максим. падение U'
                }
                SpinBox {//TextField { // Sumpad
                    id: sumpad
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' B'
                }
                Label { // n1
                    id: n1_3
                    text: 'Ток уставки ЛВ (min)'
                }
                SpinBox {//TextField { // iustLB
                    id: iustLB
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' A'
                }
                Label { // n1
                    id: n1_4
                    text: 'Падение U в проводе КС в одном направл.'
                }
                SpinBox {//TextField { // padUkc
                    id: padUkc
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' B'
                }
                Label { // n1
                    id: n1_5
                    text: 'Падение U на полож. кабеле'
                }
                SpinBox {//TextField { // padUkabpol
                    id: padUkabpol
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' B'
                }
                Label { // n1
                    id: n1_6
                    text: 'Падение Uна отриц. кабеле'
                }
                SpinBox {//TextField { // padUkabotr
                    id: padUkabotr
                    Layout.fillWidth: true
                    minimumValue: 0
                    maximumValue: 100000
                    decimals: 2
                    suffix: ' B'
                }
                Item{ Layout.fillHeight: true }
            }
        }
        GroupBox {
            id: nd5
            title: 'Рекомендуемые и справочные значения.'
            Layout.fillWidth: true
            ColumnLayout {
                width: parent.width
                GroupBox {
                    Layout.fillWidth: true

                    title: 'Рекомендуемые значения.'
                    GridLayout {
                        width: parent.width
                        columns: 2
                        Label { // n1
                            id: n1_7
                            Layout.fillWidth: true
                            text: 'Рекомендуемое значение частоты движения\nпри падении напряжения на КС =90 В'
                            color: '#ff0000'
                        }
                        SpinBox {//TextField { // nnom
                            id: nnom
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                        }
                        Label { // n221
                            Layout.fillWidth: true
                            id: n221_1
                            text: 'Общее, рекомендуемое, кол-во машин\nна участке расчёта, приведённое к ЗИУ-9.'
                        }
                        SpinBox {//TextField { // nnom2
                            id: nnom2
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                        }
                        Label { // n1
                            id: n1_9
                            Layout.fillWidth: true
                            text: 'Ток участка'

                        }
                        SpinBox {//TextField { // inom
                            id: inom
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' A'
                        }
                        Label { // n1
                            id: n1_10
                            Layout.fillWidth: true
                            text: 'Плотность тока'
                        }
                        SpinBox {//TextField { // iplotnom
                            id: iplotnom
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' A/мм²'
                        }
                    }
                }
                GroupBox {
                    title: 'Максимальные значения.'
                    Layout.fillWidth: true
                    GridLayout {
                        columns: 2
                        width: parent.width
                        Label { // n1
                            id: n1_8
                            Layout.fillWidth: true
                            text: 'Макс. допустимое значение частоты движения\nпри макс. падении U на КС =160 В '
                            color: '#0000ff'
                        }
                        SpinBox {//TextField { // nmax
                            id: nmax
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                        }
                        Label { // n221
                            id: n221_2
                            Layout.fillWidth: true
                            text: 'Макс. допустимое  кол-во машин\nна участке расчёта, приведённое к ЗИУ-9.'
                        }
                        SpinBox {//TextField { // nmax1
                            id: nmax1
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                        }
                        Label { // n1
                            id: n1_11
                            Layout.fillWidth: true
                            text: 'Ток участка'
                        }
                        SpinBox {//TextField { // imax
                            id: imax
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' A'
                        }
                        Label { // n1
                            id: n1_12
                            Layout.fillWidth: true
                            text: 'Плотность тока'
                        }
                        SpinBox {//TextField { // iplotmax
                            id: iplotmax
                            minimumValue: 0
                            maximumValue: 100000
                            decimals: 2
                            suffix: ' A/мм²'
                        }
                    }
                }
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            GroupBox {
                id: n333
                Layout.fillWidth: true
                title: 'Значение тока КЗ и уставки ЛВ для данного участка КС, в удалённой точке.'
                GridLayout {
                    columns: 2
                    anchors.fill: parent
                    Label { // n5
                        id: n5
                        Layout.fillWidth: true
                        text: 'Ток КЗ на удалении длины участка'
                    }
                    SpinBox {//TextField { // irez
                        id: irez
                        minimumValue: 0
                        maximumValue: 100000
                        decimals: 2
                        suffix: ' A'
                    }
                    Label { // n6
                        id: n6
                        Layout.fillWidth: true
                        text: 'Соответствующий ток уставки ЛВ'

                    }
                    SpinBox {//TextField { // iust
                        id: iust
                        minimumValue: 0
                        maximumValue: 100000
                        decimals: 2
                        suffix: ' A'
                    }
                }
            }
            Button { // nag
                id: nag
                text: 'Расчёт.'
                Layout.alignment: Text.AlignHCenter
                onClicked: {
                    calc.fNag_Click()
                    calc.fr1_Click()
                }
            }
            Button {
                text: 'Сбросить'
                Layout.alignment: Text.AlignHCenter
                onClicked: {
                    calc.reset()
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                Label { // os1
                    id: os1_4
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: 'Ошибка !'
//                    font.family: 'MS Sans Serif'
                    font.weight: Font.Bold
                    font.pointSize: 14
                    color: '#f00'
                    visible: false
                }
                Label { // os1
                    id: os1_0
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: 'Не задано значение частоты движения.'
//                    font.family: 'MS Sans Serif'
                    font.weight: Font.Bold
                    font.pointSize: 14
                    visible: false
                }
                Label { // os1
                    id: os1_1
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: 'Не задано значение длины участка КС.'
//                    font.family: 'MS Sans Serif'
                    font.weight: Font.Bold
                    font.pointSize: 14
                    visible: false
                }
                Label { // os1
                    id: os1_2
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: 'Не задано значение длины полож. кабеля.'
//                    font.family: 'MS Sans Serif'
                    font.weight: Font.Bold
                    font.pointSize: 14
                    visible: false
                }
                Label { // os1
                    id: os1_3
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: 'Не задано значение длины отриц. кабеля.'
//                    font.family: 'MS Sans Serif'
                    font.weight: Font.Bold
                    font.pointSize: 14
                    visible: false
                }
            }
            Item{
                Layout.fillHeight: true
            }
        }
    }
}





