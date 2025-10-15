#include "guidepathsmoother.h"



GuidePath GuidePathSmoother::source() const
{
    return m_source;
}

void GuidePathSmoother::setSource(GuidePath source)
{
    if (m_source == source)
        return;

    m_source = source;
    update();
}

qreal GuidePathSmoother::radius() const
{
    return m_radius;
}

void GuidePathSmoother::setRadius(qreal radius)
{
    if (m_radius == radius)
        return;

    m_radius = radius;
    update();
}

qreal GuidePathSmoother::precision() const
{
    return m_precision;
}

void GuidePathSmoother::setPrecision(qreal precision)
{
    if (m_precision == precision)
        return;

    m_precision = precision;
    update();
}

qreal GuidePathSmoother::startPos() const
{
    return m_startPos;
}

void GuidePathSmoother::setStartPos(qreal startPos)
{
    if (m_startPos == startPos)
        return;

    m_startPos = startPos;
    update();
}

qreal GuidePathSmoother::endPos() const
{
    return m_endPos;
}

void GuidePathSmoother::setEndPos(qreal endPos)
{
    if (m_endPos == endPos)
        return;

    m_endPos = endPos;
    update();
}

GuidePath GuidePathSmoother::result() const
{
    return m_result;
}

void GuidePathSmoother::update()
{
    if (m_source.isEmpty())
        return;
    qreal step = 0;
    m_result.clear();
    if (m_endPos < 0)
        m_endPos = m_source.length();
    //    ret.appendPoint(headNode().point());
    PathRunner vertexRunner(&m_source, m_startPos);
    PathRunner smoothRunner(&m_source, m_startPos);
    m_result.appendPoint(vertexRunner.point());
    vertexRunner.hopForward();

    bool hasPoint = true;
    while (!vertexRunner.atTail()) {
        float turn = vertexRunner.fastTurn();
        //        float turn = vertexRunner.fastTurn(ret.isEmpty() ? nullptr : &ret.lastVectorNode());
        if (turn < 0)
            turn *= -0.4; //
        if (turn > 0) {
            //            if (!step || )

            step = m_radius / (turn * m_precision) * 6;
            //            qreal step = radius / (qSqrt(turn) * precision);
            qreal negLimit = vertexRunner.position() - m_radius / 2;
            qreal posLimit = vertexRunner.position() + m_radius / 2;
            //                        qreal negLimit = vr.position() - qMin(vr.lengthBehind(), radius / 2);
            //                        qreal posLimit = vr.position() + qMin(vr.lengthAhead(), radius / 2);
            //                        qreal delta = negLimit - pr.position();
            if (negLimit > smoothRunner.position()) {
                //                qDebug() << "span:" << posLimit - negLimit;
                smoothRunner.moveForward(negLimit - smoothRunner.position());
                hasPoint = false;
            }
            if (smoothRunner.position() + step > posLimit) {
                //                ret.appendPoint(vertexRunner.point());
                //                VectorNode n = ret.createNextNode(vertexRunner.point() + vertexRunner.roundingDelta(radius));
                ////                qDebug() << ret.count() << "node alone, vecnode len:" << n.length() << "step" << step << "pos" << smoothRunner.position() << "vert pos" << vertexRunner.position() << "turn" << turn;
                //                if (vertexRunner.position() > smoothRunner.position()) {
                //                    ret.appendNode(n);
                //                }
                //                if (turn < 2 / precision) {
                //                    if (!step || step < n.length()) {
                //    //                    vertexRunner.hopForward();
                //                        continue;
                //                    }
                //                }
                vertexRunner.hopForward();
                continue;
            }
            while (!smoothRunner.atTail() && smoothRunner.position() < posLimit) {
                if (!hasPoint) {
                    const VectorNode &reference = m_result.isEmpty() ? *smoothRunner.vnode() : m_result.lastVNode();
                    QPointF pt = smoothRunner.point() + smoothRunner.roundingDelta(m_radius);
                    //                    currentPt = r.point() + r.roundingDelta(radius, &curveFwd, &curveBack);
                    VectorNode n = m_result.createNextNode(pt);
                    if (step / n.length() < 10) {

                        //                    if (n.length() < )
                        float sharpness = qAbs(n.fastAngleTo(reference));
//                        qDebug() << "at node" << ret.count()
//                                 << "\npos" << smoothRunner.position()
//                                 << "\nturn sharpness" << sharpness
//                                 << "\nstepratio" << step /n.length()
//                                 << "\nvec length:" << n.length();

                        m_result.appendNode(n);
                        hasPoint = true;
                        if (sharpness == 0) {
                            //                    } else if (vertexRunner.position() < smoothRunner.position()) {
//                            qDebug() << "nothing to smooth at node" << ret.count();
                            break; // go to next vertex if current rounding is complete
                        }
                    }
                }
                if (!smoothRunner.atTail() && (smoothRunner.position() + step) <= posLimit) {
                    smoothRunner.moveForward(step);
                    hasPoint = false;
                } else {
                    break;
                }
            }
        }
        vertexRunner.hopForward();
    }
    //    qDebug() << vr.point() << pr.point() << vr.currentNodeType() << pr.currentNodeType();
    m_result.appendPoint(vertexRunner.point());
    //    qDebug() << "Rounding done" << ret.count() << (ret.head() == ret.tail());
}

