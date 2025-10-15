#include "boundaryobject.h"

#include <QPainter>

BoundaryObject::BoundaryObject(QGraphicsItem *parent) : GraphicsObject(parent)
{

}

QRectF BoundaryObject::boundingRect() const
{
    if (!m_prevSection || !m_nextSection)
        return QRectF();
//    auto pr = m_output.posResult().headRunner();
    auto pr = m_nextSection->segment(m_uidTo).posResult().headRunner();
//    qDebug() << "bounding rect" << pr.point() << m_pp << pr.angle() << m_pa;
    QTransform t;
    t.translate(pr.point().x(), pr.point().y());
    t.rotate(pr.angle());

//    t.translate(m_pp.x(), m_pp.y());
//    t.rotate(m_pa);

    //////////////////////// sizes
    qreal gapSize = m_iGap * m_scaleFactor;
    qreal heightSize = m_iHeight * m_scaleFactor;

    qreal triangleOffsetFactor = 3.0;
    qreal triangleWidthFactor = 1.4;
    qreal triangleHeightFactor = 0.8;
    qreal triangleOffset = gapSize * triangleOffsetFactor;
    qreal triangleWidth = heightSize * triangleWidthFactor;
    qreal triangleHeight = heightSize * triangleHeightFactor;

    qreal breakerWidthFactor = 3.0;
    qreal breakerHeightFactor = 0.9;
    qreal breakerWidth = gapSize * breakerWidthFactor;
    qreal breakerHeight = heightSize * breakerHeightFactor;
    ////////////////////////

    return t.mapRect( QRectF(-gapSize * 2 - triangleWidth, -heightSize, 2* (gapSize * 2 + triangleWidth), heightSize * 2) );
}

void BoundaryObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    if( !m_ok ) return;
    if( m_prevSection == nullptr || m_nextSection == nullptr )
        return;

//qDebug() << "p runner at vector"
//         << m_pr.atVector()
//         << m_pr.atVertex()
//         << m_nextSection->segment(m_uidTo).posResult().isEmpty()
//         << m_nextSection->segment(m_uidTo).posResult().length()
//         << m_pr.point()
//         << m_pr.angle();

    if( m_prevSection->doubleWire() || m_nextSection->doubleWire() )
        paintTrolleyBreaker(painter);
    else
        paintTramBreaker(painter);


//    painter->fillRect(boundingRect(), QColor( 128, 128, 255, 64 ) ); // bounding rect for dbg
//    painter->strokePath(shape(), QPen() );
//    m_ok = false;
}

void BoundaryObject::paintTramBreaker(QPainter *painter)
{
    if( m_posLink != NoBreaker ) {

        painter->save();
//        static Runner pr = m_nextSection->segment(m_uidTo).posResult().headRunner();
//        if( !m_nextSection->isShapeReady() )
//            pr = m_nextSection->segment(m_uidTo).posResult().headRunner(); // position painter on pos wire
//        auto pr = m_output.posResult().headRunner(); // position painter on tram wire

//        painter->translate(m_pr.point());
//        painter->rotate(m_pr.angle());

        painter->translate(m_pp);
        painter->rotate(m_pa);

        auto penI = m_prevSection->posPen();
        auto penO = m_nextSection->posPen();
        paintTramLikeBreaker(painter, penI, penO, m_posLink);

        painter->restore();
    }
}

void BoundaryObject::paintTrolleyBreaker(QPainter *painter)
{
    if( m_posLink != NoBreaker && m_negLink != NoBreaker ) {
        // both big breaker
        painter->save();
//        static Runner pr = m_nextSection->segment(m_uidTo).posResult().headRunner();
//        static Runner nr = m_nextSection->segment(m_uidTo).negResult().headRunner();
//        if( !m_nextSection->isShapeReady() ) {
//            pr = m_nextSection->segment(m_uidTo).posResult().headRunner(); // position painter on pos wire
//            nr = m_nextSection->segment(m_uidTo).negResult().headRunner(); // position painter on neg wire
//        }

//        auto r = ( m_pr.point() + m_nr.point() ) * 0.5;
//        auto a = ( pr.angle() + nr.angle() ) * 0.5;
//        auto a = m_pr.angle();


        auto r = ( m_pp + m_np ) * 0.5;
//        auto a = ( pr.angle() + nr.angle() ) * 0.5;
        auto a = m_pa;
        painter->translate(r);
        painter->rotate(a);

        //////////////////////// pens
        auto penPI = m_prevSection->posPen();
        auto penNI = m_prevSection->negPen();
        auto penPO = m_nextSection->posPen();
        auto penNO = m_nextSection->negPen();
        GPen penB;
        qreal s = scene()->viewportScale();
        penB.copy(penPI);

        // 1 / (viewportScale > 4 ? viewportScale : 2 * Math.sqrt(viewportScale))
        if (s < 4.0) {
            qreal w = 0.5 * qSqrt(s);
            penB.setWidth(qMax(penPI.width() * w, 1.0));
        }
        penB.setColor(Qt::black);
        ////////////////////////

        //////////////////////// sizes
        qreal gapSize = m_iGap * m_scaleFactor;
        qreal heightSize = m_iHeight * m_scaleFactor;

        qreal triangleOffsetFactor = 3.0;
        qreal triangleWidthFactor = 1.8;
        qreal triangleHeightFactor = 1.4;
        qreal triangleOffset = gapSize * triangleOffsetFactor;
        qreal triangleWidth = heightSize * triangleWidthFactor;
        qreal triangleHeight = heightSize * triangleHeightFactor;

        qreal breakerWidthFactor = 1.8;
        qreal breakerHeightFactor = 0.9;
        qreal breakerWidth = gapSize * breakerWidthFactor;
        qreal breakerHeight = heightSize * breakerHeightFactor;
        ////////////////////////

        //////////////////////// draw positive in wire breaker
        QPainterPath breakerPI;
        breakerPI.moveTo( -gapSize,  heightSize );
        breakerPI.lineTo( -gapSize, -heightSize );
        painter->strokePath(breakerPI, penB );
        ////////////////////////

        //////////////////////// draw positive out wire breaker
        QPainterPath breakerPO;
        breakerPO.moveTo(  gapSize,  heightSize );
        breakerPO.lineTo(  gapSize, -heightSize );
        painter->strokePath(breakerPO, penB);
        ////////////////////////





        //////////////////////// draw triangles

        if( m_posFeedPass != NoPass && m_negFeedPass == NoPass ) {
            // draw big pos triangle
            auto side = m_posFeedPass == Out2In ? -1 : 1;
            QPainterPath t;
            t.moveTo( side * triangleOffset,                  triangleHeight );
            t.lineTo( side *( triangleOffset + triangleWidth),  0 );
            t.lineTo( side * triangleOffset,                 -triangleHeight );
            t.lineTo( side * triangleOffset,                  triangleHeight );
            painter->fillPath( t, m_posFeedPass == Out2In ? QColor( penPO.color() ) : QColor( penPI.color() ) );
        }
        else if( m_posFeedPass == NoPass && m_negFeedPass != NoPass ) {
            // draw big neg triangle
            auto side = m_negFeedPass == Out2In ? -1 : 1;
            QPainterPath t;
            t.moveTo( side * triangleOffset,                  triangleHeight );
            t.lineTo( side * (triangleOffset + triangleWidth),  0 );
            t.lineTo( side * triangleOffset,                 -triangleHeight );
            t.lineTo( side * triangleOffset,                  triangleHeight );
            painter->fillPath( t, m_negFeedPass == Out2In ? QColor( penNO.color() ) : QColor( penNI.color() ) );
        }
        else if( m_posFeedPass != NoPass && m_negFeedPass != NoPass ) {
            // draw two small triangles
            auto pside = m_posFeedPass == Out2In ? -1 : 1;
            auto nside = m_negFeedPass == Out2In ? -1 : 1;

            QPainterPath nt;
            nt.moveTo( nside * triangleOffset,                  triangleHeight + triangleHeight * 0.5);
            nt.lineTo( nside * (triangleOffset + triangleWidth),  triangleHeight * 0.4 + triangleHeight * 0.25);
            nt.lineTo( nside * triangleOffset,                 -triangleHeight * 0.2 );
            nt.lineTo( nside * triangleOffset,                  triangleHeight );
            painter->fillPath( nt, m_negFeedPass == Out2In ? QColor( penNO.color() ) : QColor( penNI.color() ) );

            QPainterPath pt;
            pt.moveTo( pside * triangleOffset,                  -triangleHeight - triangleHeight * 0.5);
            pt.lineTo( pside * (triangleOffset + triangleWidth),  -triangleHeight * 0.4 - triangleHeight * 0.25);
            pt.lineTo( pside * triangleOffset,                   triangleHeight * 0.2 );
            pt.lineTo( pside * triangleOffset,                  -triangleHeight );
            painter->fillPath( pt, m_posFeedPass == Out2In ? QColor( penPO.color() ) : QColor( penPI.color() ) );
        }
        ////////////////////////






        //////////////////////// draw closed pos breaker
        if( m_posLink == ClosedBreaker ) {
            QPainterPath breakerP;
            breakerP.moveTo( -breakerWidth, -breakerHeight );
            breakerP.lineTo(  breakerWidth, -breakerHeight );
            painter->strokePath(breakerP, penB);
        }
        ////////////////////////


        //////////////////////// draw closed neg breaker
        if( m_negLink == ClosedBreaker ) {
            QPainterPath breakerN;
            breakerN.moveTo( -breakerWidth,  breakerHeight );
            breakerN.lineTo(  breakerWidth,  breakerHeight );
            painter->strokePath(breakerN, penB);
        }
        ////////////////////////

        painter->restore();
    }
    else if( m_posLink != NoBreaker && m_negLink == NoBreaker ) {
        // tramlike breaker
        painter->save();
        auto pr = m_output.posResult().headRunner(); // position painter on tram wire
//        painter->translate(pr.point());
//        painter->rotate(pr.angle());

        painter->translate(m_pp);
        painter->rotate(m_pa);
        auto penI = m_prevSection->posPen();
        auto penO = m_nextSection->posPen();
        paintTramLikeBreaker(painter, penI, penO, m_posLink);
        painter->restore();
    }
    else if( m_posLink == NoBreaker && m_negLink != NoBreaker ) {
        // tremlike breaker on negative
        painter->save();
        auto pr = m_output.negResult().headRunner(); // position painter on tram wire
//        painter->translate(pr.point());
//        painter->rotate(pr.angle());
        painter->translate(m_np);
        painter->rotate(m_na);

        auto penI = m_prevSection->negPen();
        auto penO = m_nextSection->negPen();
        paintTramLikeBreaker(painter, penI, penO, m_negLink);
        painter->restore();
    }
}

void BoundaryObject::paintTramLikeBreaker(QPainter *painter, GPen penI, GPen penO, LinkType linkType)
{bool colored = false; // do we need breakers to be colored?

    //////////////////////// pens
    GPen penB;
    penB.copy(penO);
    qreal s = scene()->viewportScale();

    // 1 / (viewportScale > 4 ? viewportScale : 2 * Math.sqrt(viewportScale))
    if (s < 4.0) {
        qreal w = 0.5 * qSqrt(s);
        penB.setWidth(qMax(penO.width() * w* 0.75, 1.0));
    }
    penB.setColor(Qt::black);
    ////////////////////////


    //////////////////////// sizes
    qreal gapSize = m_iGap * m_scaleFactor;
    qreal heightSize = m_iHeight * m_scaleFactor;

    qreal triangleOffsetFactor = 3.0;
    qreal triangleWidthFactor = 1.8;
    qreal triangleHeightFactor = 1.4;
    qreal triangleOffset = gapSize * triangleOffsetFactor;
    qreal triangleWidth = heightSize * triangleWidthFactor;
    qreal triangleHeight = heightSize * triangleHeightFactor;

    qreal breakerWidthFactor = 3.0;
    qreal breakerHeightFactor = 0.9;
    qreal breakerWidth = gapSize * breakerWidthFactor;
    qreal breakerHeight = heightSize * breakerHeightFactor;
    ////////////////////////


    //////////////////////// draw positive in wire breaker
    QPainterPath breakerPI;
    breakerPI.moveTo( -gapSize,  heightSize );
    breakerPI.lineTo( -gapSize, -heightSize );
    painter->strokePath(breakerPI, m_posOperation != OperationCompleted ? penI : penB );
    ////////////////////////

    //////////////////////// draw positive out wire breaker
    QPainterPath breakerPO;
    breakerPO.moveTo(  gapSize,  heightSize );
    breakerPO.lineTo(  gapSize, -heightSize );
    painter->strokePath(breakerPO, m_posOperation != OperationCompleted ? penO : penB);
    ////////////////////////


    //////////////////////// draw triangles
    if( m_posFeedPass == In2Out ) {
        QPainterPath t;
        t.moveTo(  triangleOffset,                  triangleHeight );
        t.lineTo(  triangleOffset + triangleWidth,  0 );
        t.lineTo(  triangleOffset,                 -triangleHeight );
        t.lineTo(  triangleOffset,                  triangleHeight );
        painter->fillPath( t, QColor( m_posOperation == OperationCompleted ? penI.color() : penO.color() ) );
    }
    else if( m_posFeedPass == Out2In ) {
        QPainterPath t;
        t.moveTo(  -triangleOffset,                 triangleHeight );
        t.lineTo(  -triangleOffset - triangleWidth, 0 );
        t.lineTo(  -triangleOffset,                -triangleHeight );
        t.lineTo(  -triangleOffset,                 triangleHeight );
        painter->fillPath( t, QColor( m_posOperation == OperationCompleted ? penO.color() : penI.color() ) );
    }
    ////////////////////////

    //////////////////////// draw closed breaker
    if( linkType == ClosedBreaker ) {
        QPainterPath breakerP;
        breakerP.moveTo( -breakerWidth, -breakerHeight );
        breakerP.lineTo(  breakerWidth, -breakerHeight );
        painter->strokePath(breakerP, penB);
    }
    ////////////////////////



}

QPainterPath BoundaryObject::shape() const
{
    QPainterPath p;

    p.moveTo( boundingRect().topLeft() );
    p.lineTo( boundingRect().topRight() );
    p.lineTo( boundingRect().bottomRight() );
    p.lineTo( boundingRect().bottomLeft() );
    p.lineTo( boundingRect().topLeft() );
    return p;
}

GuideView *BoundaryObject::prevSection() const
{
    return m_prevSection;
}

GuideView *BoundaryObject::nextSection() const
{
    return m_nextSection;
}

Guide BoundaryObject::negInput() const
{
    return m_negInput;
}

Guide BoundaryObject::negOutput() const
{
    return m_negOutput;
}

Guide BoundaryObject::posInput() const
{
    return m_posInput;
}

Guide BoundaryObject::posOutput() const
{
    return m_posOutput;
}

BoundaryObject::LinkType BoundaryObject::negLink() const
{
    return m_negLink;
}

BoundaryObject::LinkType BoundaryObject::posLink() const
{
    return m_posLink;
}

BoundaryObject::FeedPass BoundaryObject::negFeedPass() const
{
    return m_negFeedPass;
}

BoundaryObject::FeedPass BoundaryObject::posFeedPass() const
{
    return m_posFeedPass;
}

void BoundaryObject::setPrevSection(GuideView *prevSection)
{
    qDebug() << "prevSection" << prevSection;
    if (m_prevSection == prevSection)
        return;

    m_prevSection = prevSection;
    emit prevSectionChanged(prevSection);
    connect(m_prevSection, &GuideView::shapeReady, this, &BoundaryObject::decoratePrev);
}

void BoundaryObject::setNextSection(GuideView *nextSection)
{
    if (m_nextSection == nextSection)
        return;

    m_nextSection = nextSection;
//    if( !m_nextSection->segment(m_uidTo).posResult().isEmpty() )
        m_pr = m_nextSection->segment(m_uidTo).posResult().headRunner();
        m_pp = m_pr.point();
        m_pa = m_pr.angle();
//    if( !m_nextSection->segment(m_uidTo).negResult().isEmpty() )
        m_nr = m_nextSection->segment(m_uidTo).negResult().headRunner();
        m_np = m_nr.point();
        m_na = m_nr.angle();
    emit nextSectionChanged(nextSection);
    connect(m_nextSection, &GuideView::shapeReady, this, &BoundaryObject::decorateNext);
}

void BoundaryObject::setNegInput(Guide negInput)
{
    if (m_negInput == negInput)
        return;

    m_negInput = negInput;
    emit negInputChanged(negInput);
}

void BoundaryObject::setNegOutput(Guide negOutput)
{
    if (m_negOutput == negOutput)
        return;

    m_negOutput = negOutput;
    emit negOutputChanged(negOutput);
}

void BoundaryObject::setPosInput(Guide posInput)
{
    if (m_posInput == posInput)
        return;

    m_posInput = posInput;
    emit posInputChanged(posInput);
}

void BoundaryObject::setPosOutput(Guide posOutput)
{
    if (m_posOutput == posOutput)
        return;

    m_posOutput = posOutput;
    emit posOutputChanged(posOutput);
}

void BoundaryObject::setNegLink(BoundaryObject::LinkType negLink)
{
    if (m_negLink == negLink)
        return;

    m_negLink = negLink;
    emit negLinkChanged(negLink);
}

void BoundaryObject::setPosLink(BoundaryObject::LinkType posLink)
{
    if (m_posLink == posLink)
        return;

    m_posLink = posLink;
    emit posLinkChanged(posLink);
}

void BoundaryObject::setNegFeedPass(BoundaryObject::FeedPass negFeedPass)
{
    if (m_negFeedPass == negFeedPass)
        return;

    m_negFeedPass = negFeedPass;
    emit negFeedPassChanged(negFeedPass);
}

void BoundaryObject::setPosFeedPass(BoundaryObject::FeedPass posFeedPass)
{
    if (m_posFeedPass == posFeedPass)
        return;

    m_posFeedPass = posFeedPass;
    emit posFeedPassChanged(posFeedPass);
}

void BoundaryObject::decoratePrev()
{
    if (!m_prevSection->isShapeReady())
        return;

    //////////////////////// sizes
    qreal gapSize = m_iGap * m_scaleFactor;
    qreal heightSize = m_iHeight * m_scaleFactor;

    qreal triangleOffsetFactor = 3.0;
    qreal triangleWidthFactor = 1.2;
    qreal triangleHeightFactor = 0.7;
    qreal triangleOffset = gapSize * triangleOffsetFactor;
    qreal triangleWidth = heightSize * triangleWidthFactor;
    qreal triangleHeight = heightSize * triangleHeightFactor;

    qreal breakerWidthFactor = 3.0;
    qreal breakerHeightFactor = 0.9;
    qreal breakerWidth = gapSize * breakerWidthFactor;
    qreal breakerHeight = heightSize * breakerHeightFactor;
    ////////////////////////

    qreal trim = gapSize * 0.5 + 0.2 * m_scaleFactor;
    if( m_posFeedPass == Out2In || m_negFeedPass == Out2In )
        trim += gapSize + 0.4 * m_scaleFactor;

    m_prevSection->segment(m_uidFrom).posResult().trimEnd( trim );
    m_prevSection->segment(m_uidFrom).negResult().trimEnd( trim );
}

void BoundaryObject::decorateNext()
{
    if (!m_nextSection->isShapeReady())
        return;
    if( !m_nextSection->segment(m_uidTo).posResult().isEmpty() ) {
        m_pr = m_nextSection->segment(m_uidTo).posResult().headRunner();
        m_pp = m_pr.point();
        m_pa = m_pr.angle();
    }
    if( !m_nextSection->segment(m_uidTo).negResult().isEmpty() ) {
        m_nr = m_nextSection->segment(m_uidTo).negResult().headRunner();
        m_np = m_nr.point();
        m_na = m_nr.angle();
    }
//    m_ok = true;



    //////////////////////// sizes
    qreal gapSize = m_iGap * m_scaleFactor;
    qreal heightSize = m_iHeight * m_scaleFactor;

    qreal triangleOffsetFactor = 3.0;
    qreal triangleWidthFactor = 1.4;
    qreal triangleHeightFactor = 0.8;
    qreal triangleOffset = gapSize * triangleOffsetFactor;
    qreal triangleWidth = heightSize * triangleWidthFactor;
    qreal triangleHeight = heightSize * triangleHeightFactor;

    qreal breakerWidthFactor = 3.0;
    qreal breakerHeightFactor = 0.9;
    qreal breakerWidth = gapSize * breakerWidthFactor;
    qreal breakerHeight = heightSize * breakerHeightFactor;
    ////////////////////////

    qreal trim = gapSize * 0.5 + 0.2 * m_scaleFactor;

    if( m_posFeedPass == In2Out || m_negFeedPass == In2Out )
        trim += gapSize + 0.4 * m_scaleFactor;

    m_nextSection->segment(m_uidTo).posResult().trimStart( trim );
    m_nextSection->segment(m_uidTo).negResult().trimStart( trim );
    if (m_prevSection->isShapeReady())
        update();
}



