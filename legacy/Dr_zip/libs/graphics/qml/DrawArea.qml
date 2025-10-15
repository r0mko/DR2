import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import Dr 1.0
import Dr.Graphics 1.0

Item {
    id: drawArea
    clip: true

    property alias backgroundColor: m_background.color
    property alias backgroundVisible: m_background.visible

    property alias scrollArea: m_scrollArea
    property alias scene: m_scene
    property alias mouse: m_mouseArea
    property alias scale: m_scene.scale
    property real targetScale
    property alias scaleAnimation: scaleAnim
    property alias grid: m_grid

    property real maxScale: 64
    default property alias sceneitems: m_scene.data

    Rectangle {
        id: m_background
        color: "#222"
        anchors.fill: m_scrollArea
        Behavior on color { ColorAnimation { duration: 250 }}
    }

    BackgroundGrid {
        id: m_grid
        anchors.fill: m_scrollArea

        pos: Qt.point(m_scrollArea.contentX, m_scrollArea.contentY)
        step: 500
        divisor: 5
        minimumStep: 4
        scaleFactor: m_scene.scale

        fillColor: '#000'
        gridColor: '#fff'
        fillOpacity: 0
        gridMinOpacity: 0.05

    }

    function positionScene( x, y, zoomstep ){ // scale and position whole scene ( for restoring )
        var seriesIdx = Math.abs(zoomstep % m_scrollArea.scaleSeries.length)
        var power = Math.floor(zoomstep / m_scrollArea.scaleSeries.length)
        var scale = Math.pow(m_scrollArea.scaleBase, power) * m_scrollArea.scaleSeries[seriesIdx] // current scale
        m_scrollArea.zoomStep = zoomstep
        m_scene.scale = scale
        m_scrollArea.contentX = x
        m_scrollArea.contentY = y
    }
    function centerScene(){ // center scene in view ( no scale )
        m_scrollArea.contentX = ( m_scrollArea.contentWidth - drawArea.width ) * 0.5
        m_scrollArea.contentY = ( m_scrollArea.contentHeight - drawArea.height ) * 0.5
    }
    function centerSceneOn(x, y){ // center scene in view ( no scale )
        m_scrollArea.contentX = x * drawArea.scale - drawArea.width * 0.5
        m_scrollArea.contentY = y * drawArea.scale - drawArea.height * 0.5
    }
    function centerOn( rect, scale ){ // senter rect(scene coords) in view; if scale is undefined - current scene scale used
        if( scale === undefined ) scale = drawArea.scale
        m_scrollArea.contentX = scale * rect.x + ( scale * rect.width - drawArea.width ) * 0.5
        m_scrollArea.contentY = scale * rect.y + ( scale * rect.height - drawArea.height ) * 0.5
    }
    function fitInView( rect ){ // fits rect in a view
        var czs = m_scrollArea.zoomStep // current zoomStep
        var seriesIdx = Math.abs(czs % m_scrollArea.scaleSeries.length)
        var power = Math.floor(czs / m_scrollArea.scaleSeries.length)
        var scale = Math.pow(m_scrollArea.scaleBase, power) * m_scrollArea.scaleSeries[seriesIdx] // current scale

        var t = 0
        if( canBeFitInView( rect.width * scale, rect.height * scale ) ){
            var psc
            while( canBeFitInView( rect.width * scale, rect.height * scale ) ){
                psc = scale
                t++
                seriesIdx = Math.abs( ( czs + t ) % m_scrollArea.scaleSeries.length)
                power = Math.floor( ( czs + t ) / m_scrollArea.scaleSeries.length)
                scale = Math.pow(m_scrollArea.scaleBase, power) * m_scrollArea.scaleSeries[seriesIdx]
            }
            t--
            scale = psc
        }else{
            while( !canBeFitInView( rect.width * scale, rect.height * scale ) ){
                t--
                seriesIdx = Math.abs( ( czs + t ) % m_scrollArea.scaleSeries.length)
                power = Math.floor( ( czs + t ) / m_scrollArea.scaleSeries.length)
                scale = Math.pow(m_scrollArea.scaleBase, power) * m_scrollArea.scaleSeries[seriesIdx]
            }
        }

        if( t !== 0 ){
            m_scrollArea.zoomStep = t + czs;
            m_scene.scale = scale
        }

        centerOn( rect, scale )
    }
    function canBeFitInView( w, h ){ // is this size smaller than drawArea
        if( w < drawArea.width && h < drawArea.height )
            return true
        return false
    }
    function intersects( rect ){ // does rect intersect drawarea?
        var r = rect
        var d = Qt.rect( drawArea.x, drawArea.y, drawArea.width, drawArea.height )
//        console.log( r.x, r.y, r.width, r.height )
//        console.log( d.x, d.y, d.width, d.height )

        var l1 = r.x;
        var r1 = r.x;
        if (r.width < 0)
            l1 += r.width;
        else
            r1 += r.width;
        if (l1 == r1) // null rect
            return false;

        var l2 = d.x;
        var r2 = d.x;
        if (d.width < 0)
            l2 += d.width;
        else
            r2 += d.width;
        if (l2 == r2) // null rect
            return false;

        if (l1 >= r2 || l2 >= r1)
            return false;

        var t1 = r.y;
        var b1 = r.y;
        if (r.height < 0)
            t1 += r.height;
        else
            b1 += r.height;
        if (t1 === b1) // null rect
            return false;

        var t2 = d.y;
        var b2 = d.y;
        if (d.height < 0)
            t2 += d.height;
        else
            b2 += d.height;
        if (t2 == b2) // null rect
            return false;

        if (t1 >= b2 || t2 >= b1)
            return false;

        return true;
    }

    function ensureVisible( rect, scale ){
        if( scale === undefined ) scale = m_scene.scale
        var r = rect
        var d = Qt.rect( drawArea.x, drawArea.y, drawArea.width, drawArea.height )
        var part = 50

        console.log( 'ensurevisible', r , d )

        if( scale * ( r.x + r.width ) <= d.x + part ){
            console.log( 'scene is too left' )
            m_scrollArea.contentX = -part + r.width * scale
        }
        if( scale * ( r.x ) >= d.width - part ){
            console.log( 'scene is too right' )
            m_scrollArea.contentX = part - d.width
        }
//        if (viewRect.right() >= right - xmargin) {
//            // need to scroll from the right
//            if (!d->leftIndent)
//                horizontalScrollBar()->setValue(int(viewRect.right() - width + xmargin + 0.5));
//        }
//        if (viewRect.top() <= top + ymargin) {
//            // need to scroll from the top
//            if (!d->topIndent)
//                verticalScrollBar()->setValue(int(viewRect.top() - ymargin - 0.5));
//        }
//        if (viewRect.bottom() >= bottom - ymargin) {
//            // need to scroll from the bottom
//            if (!d->topIndent)
//                verticalScrollBar()->setValue(int(viewRect.bottom() - height + ymargin + 0.5));
//        }
    }

    MouseArea {
        id: m_mouseArea
        anchors.fill: m_scrollArea
        acceptedButtons: Qt.NoButton
        property point scenePos: toScenePos(mouseX, mouseY)
        property real sceneX: scenePos.x
        property real sceneY: scenePos.y

        onPositionChanged: scenePos = toScenePos( mouseX, mouseY )
        onWheel: viewportZoom( wheel.angleDelta.y / 120, wheel.x, wheel.y )

        function mapToScene(e) {
            return toScenePos(e.x, e.y)
        }

        function toScenePos(x, y) {
            var pos = m_scene.mapFromItem(m_mouseArea, x, y)
            return Qt.point(pos.x, pos.y)
        }

        function viewportZoom(delta, originX, originY) {
            var t_zst = m_scrollArea.zoomStep + delta
            var seriesIdx = Math.abs(t_zst % m_scrollArea.scaleSeries.length)
            var power = Math.floor(t_zst / m_scrollArea.scaleSeries.length)
            var scale = Math.pow(m_scrollArea.scaleBase, power) * m_scrollArea.scaleSeries[seriesIdx]


            if( m_scene.width * scale < drawArea.width / 10 || m_scene.height * scale < drawArea.height / 10 ) return; // if scene is too small - return
            if( scale > maxScale ) return;


            m_scrollArea.zoomStep = t_zst;
            var pos = m_scene.mapFromItem(m_mouseArea, originX, originY)
            targetScale = scale
            var nx = pos.x * scale - originX
            var ny = pos.y * scale - originY
            m_scrollArea.contentX = nx
            m_scrollArea.contentY = ny
            m_scene.scale = scale

            if( !drawArea.intersects( Qt.rect( -nx, -ny, m_scene.width * scale, m_scene.height * scale ) ) ){
//                console.log( 'can\'t c shit' )
                // reposition scene to be visible
                drawArea.ensureVisible( Qt.rect( -nx, -ny, m_scene.width * scale, m_scene.height * scale ), scale )
            }
        }
    }

    Rectangle {
        id: m_bounds
        border.width: 1
        border.color: "#80406080"
        color: "transparent"
        width: m_scene.width * m_scene.scale
        height: m_scene.height * m_scene.scale
        transform: Translate {
            x: -m_scrollArea.contentX
            y: -m_scrollArea.contentY
        }
    }

    Flickable {
        id: m_scrollArea
        property var scaleSeries: [1, 1.5]
        property int scaleBase: 2
        property int zoomStep: 0
        property real scaleK: 1
        property rect viewportRect: Qt.rect(contentX, contentY, m_scrollArea.width, m_scrollArea.height)
        property list<Object> forwardKeys
        Behavior on contentX { NumberAnimation { duration: scaleAnim.duration; easing: scaleAnim.easing }}
        Behavior on contentY { NumberAnimation { duration: scaleAnim.duration; easing: scaleAnim.easing }}

        Keys.forwardTo: forwardKeys
        Keys.onSpacePressed: m_dragAction.checked = true
        Keys.onReleased: {
            if(event.key === Qt.Key_Space)
                m_dragAction.checked = false;
        }
        states: [
            State {
                name: "drag board"
                when: m_dragAction.checked
                PropertyChanges {
                    target: m_scrollArea
                    interactive: true
                }
                PropertyChanges {
                    target: m_mouseArea
                    acceptedButtons: Qt.NoButton
                    cursorShape: m_scrollArea.dragging ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                }
                PropertyChanges {
                    target: m_scene
                    enabled: false
                }
            }
        ]

        clip: true
        anchors.fill: parent
        flickDeceleration: 1000
        focus: true
        interactive: false

        rebound: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: 500
                easing.type: Easing.OutCubic
            }
        }

        topMargin: height
        bottomMargin: height
        leftMargin: width
        rightMargin: width
        contentWidth: m_scene.width * m_scene.scale
        contentHeight: m_scene.height * m_scene.scale
        pixelAligned: true

        Item {
            id: m_scene
            Behavior on scale { NumberAnimation { id: scaleAnim; duration: 350; easing.type: Easing.OutQuint }}
            transformOrigin: Item.TopLeft
//            onScaleChanged: console.log( m_scrollArea.zoomStep )
        }
    }

    Action {
        id: m_dragAction
        checkable: true
    }

    ListModel { id: m_sceneModel }
}

