#include "guidesmoother.h"

qreal GuideSmoother::radius() const
{
    return m_radius;
}

void GuideSmoother::setRadius(qreal radius)
{
    if (m_radius == radius)
        return;

    m_radius = radius;
}

qreal GuideSmoother::precision() const
{
    return m_precision;
}

void GuideSmoother::setPrecision(qreal precision)
{
    if (m_precision == precision)
        return;

    m_precision = precision;
}

Runner GuideSmoother::addCoupler(Guide::Link &&coupler, const QPointF &point)
{
    couplers.append(coupler);
    return Runner(couplers.last().connector(), point);
}

Guide GuideSmoother::smoothed(Runner *head, qreal scale, qreal threshold)
{
    Guide result;
    Runner tail(*head);
    Runner t_runner;
    //    PathRunner h_runner(tail), t_runner;
    //    t_runner = *head;
    //    while(t_runner.atVector()) {
    //        t_runner.hopForward();
    //    }
    //    qDebug() << "peek runner" << t_runner.position() << t_runner.currentNodeType();
    for (Guide::Link &c : couplers)
        c.lock();

    qreal radius = qMax(m_radius / scale, 1.0);
    radius = qMin(radius, 250.0);
    qreal delta = radius / 2;
    //    qDebug() << "Starting from node" << head->currentNodeType();
    switch (head->currentNodeType()) {
    case GuideNode::JunctionType:
        tail.moveBack(delta);

        break;
    case GuideNode::InterlinkType:
    case GuideNode::HeadType:
    case GuideNode::VectorType:
        head->moveForward(delta);
        break;
    case GuideNode::ConvergenceType:
        head->moveForward(delta * 2);
        tail.moveForward(delta);
        break;
    default:
        break;
    }
    VectorNode n;

    QPointF pt = getSmoothedPoint(&tail, radius);
    result.appendPoint(pt);
    QPointF p2;
    if (!head->atVector() && head->hasNext()) {
        tail.moveForward(head->hopForward());
    }
        if (!tail.atVector() && tail.hasNext()) {
            head->moveForward(tail.hopForward());
        }
    while (head->atVector()) {
        QPointF bpos, bneg;
        QLineF l = getNextPoint(&tail, head, radius, threshold, &bpos, &bneg);
        pt = l.p1();
        p2 = l.p2();
        n = result.createNextNode(pt);
        //        qreal weight = result.weight(n);
        //        qDebug() << "At node" << result.count() << "weight" << weight;
        if (!result.isEmpty()) {
            QPointF rv(bpos - bneg);
            qreal rw = 2 * qAbs(QPointF::dotProduct(n.normal(), rv)) * n.length() / radius;

//            qreal fw = qAbs(QPointF::dotProduct(result.lastVNode().normal(), bpos));
//            qreal bw = qAbs(QPointF::dotProduct(result.lastVNode().normal(), bneg));
//            qreal nw = qAbs(QPointF::dotProduct(result.lastVNode().normal(), n.vector()));
//            qDebug() << "at node" << result.count() << "r_w" << rw << threshold;
//            qDebug() << "at node" << result.count() << "r_w" << rw  << threshold / scale;
//            qDebug() << "at node" << result.count() << "neg_w" << bw << "pos_w" << fw << "new_w" << nw << "r_w" << rw;
            if (rw > threshold / scale)
                result.appendNode(n);
        } else
            result.appendNode(n);

        if (!head->atVector())
            break;
        if (!tail.atVector()) {
            head->strictMoveForward(tail.hopForward());
        }
    }

    GuideNode::NodeType head_typ = head->currentNodeType();
    qreal breakpos = head->position();
//    QPointF breakpoint = getSmoothedPoint(head, radius);
//    qDebug() << "Finishing with" << head_typ
//             << "pos" << head->position()
//             << "tail type" << tail.currentNodeType()
//             << "breakpos" << breakpos
//             << tail.next()->type();
    //    qDebug() << "1st stage" << head_typ;
    t_runner = *head;
    int c = 0;
    switch (head_typ) {
    case GuideNode::JunctionType: {
        break;
    }
    case GuideNode::InterlinkType:
    case GuideNode::TailType: {
        do {
            QPointF bpos, bneg;
            QLineF l = getNextPoint(&tail, head, radius, threshold, &bpos, &bneg);
            n = result.createNextNode(l.p1());
//            if (tail.currentNodeType() == head_typ) {
//                qDebug() << "Found an exit point at" << tail.position() << head_typ;
//                break;
//            }
            p2 = l.p2();
            if (tail.position() <= breakpos)
                if (!result.isEmpty() || tail.currentNodeType() != head_typ) {
//                    qDebug() << "Adding tail" << breakpos - tail.position() << n.length() << tail.currentNodeType() << "waiting for" << head_typ;
                    QPointF rv(bpos - bneg);
                    qreal rw = 2 * qAbs(QPointF::dotProduct(n.normal(), rv)) * n.length() / radius;
                    if (rw > threshold / scale)
                        result.appendNode(n);
                } else
                    result.appendNode(n);
            else {
//                qDebug() << "Go beyond break pos" << tail.position() << breakpos << n.length() << tail.hopForward();
                break;
            }
        } while (tail.currentNodeType() != head_typ);
//        result.appendPoint(breakpoint);
        break;
    }
    case GuideNode::ConvergenceType: {
        QPolygonF over;
        while (tail.currentNodeType() != GuideNode::ConvergenceType) {
            QLineF l = getNextPoint(&tail, head, radius, threshold);
            result.appendNode(result.createNextNode(l.p1()));
            over.append(l.p2());
        }
        for (QPointF pt : over)
            result.appendNode(result.createNextNode(pt));
        break;
    }
    default:
        break;
    }
    n = result.createNextNode(getSmoothedPoint(&tail, radius));
    result.appendNode(n);
    //    qDebug() << "p2" << p2;
    //    result.appendNode(result.createNextNode(p2));

    //    makeUp(h_runner, t_runner, result, scale);
    //    polishTail(tail, result, scale);

    for (Guide::Link &c : couplers)
        c.unlock();


//    qDebug() << "iter count" << count << result.count() << "tail node" << tail.currentNodeType();
    return result;
}

QPointF GuideSmoother::getSmoothedPoint(Runner *back, qreal range) const
{
    qreal backRem;
    qreal fwdRem;
    Guide wBack = back->windowBehind(range / 2, &backRem);
    Guide wFwd = back->windowAhead(range / 2, &fwdRem);
    QPointF b_cPt = back->point();
    qreal b_underShoot = backRem / range * 2;
    qreal b_overShoot = fwdRem / range * 2;
    QPointF b_negVec = wBack.weightedAverage(range, b_cPt);
    QPointF b_posVec = wFwd.weightedAverage(range, b_cPt);
    QPointF b_negPt = b_negVec * (1 - b_overShoot) + b_posVec * b_overShoot;
    QPointF b_posPt = b_posVec * (1 - b_underShoot) + b_negVec * b_underShoot;
    QPointF bp = back->atPoint() ? back->point() : b_cPt + b_negPt + b_posPt;
    return bp;
}

void GuideSmoother::update()
{
    if (m_source.isEmpty()) {
        m_result.clear();
        return;
    }
    qreal range = qMax(m_radius / m_scale, 1.0);
    range = qMin(range, 250.0);
    qreal delta = range / 2;
    Runner head = m_source.headRunner();
    Runner tail;
    switch (head.prev()->type()) {
    case GuideNode::JunctionType:
        tail.moveBack(delta);
        break;
    case GuideNode::InterlinkType:
    case GuideNode::HeadType:
    case GuideNode::VectorType:
        head.moveForward(delta);
        break;
    case GuideNode::ConvergenceType:
        head.moveForward(delta * 2);
        tail.moveForward(delta);
        break;
    default:
        qDebug() << "Impossible case:" << head.prev()->type();
        return;
    }
    qreal backRem;
    qreal fwdRem;
    qreal backCurve;
    Guide wBack = tail.windowBehind(range / 2, &backRem);
    Guide wFwd = tail.windowAhead(range / 2, &fwdRem);
    QPointF b_cPt = tail.point();
    qreal b_underShoot = backRem / range * 2;
    qreal b_overShoot = fwdRem / range * 2;
    QPointF b_negVec = wBack.weightedAverage(range, b_cPt);
    QPointF b_posVec = wFwd.weightedAverage(range, b_cPt);
    QPointF b_negPt = b_negVec * (1 - b_overShoot) + b_posVec * b_overShoot;
    QPointF b_posPt = b_posVec * (1 - b_underShoot) + b_negVec * b_underShoot;
    QPointF pt = tail.atPoint() ? tail.point() : b_cPt + b_negPt + b_posPt;

    m_result.appendPoint(pt);
    //    QPointF b_vec(b_posPt - b_negPt);
    //    QPointF f_vec(b_posPt - b_negPt);
    //    backCurve = qAbs(QPointF::dotProduct(b_vec, tailRunner->vnode()->normalF()));


    qreal weight = INFINITY;
}



QLineF GuideSmoother::getNextPoint(Runner *tailRunner, Runner *headRunner, qreal range, qreal threshold, QPointF *bNegPt, QPointF *bPosPt) const
{
    qreal thrValue = threshold * range * 0.05;
    qreal backRem;
    qreal midRem;
    qreal fwdRem;
    float backCurve;
    float frontCurve;
    QPointF b_negPt;
    QPointF b_posPt;
    QPointF f_negPt;
    QPointF f_posPt;
    QPointF b_cPt;
    QPointF f_cPt;
    qreal rstep = 0;
    qreal cstep = 0;
    qreal fstep = 0;
    if (headRunner->atPoint() && !tailRunner->atVector()) {
        if (tailRunner->c()->isVector() && tailRunner->hasNext()) {
//            qDebug() << "stall!" << headRunner->currentNodeType() << "tail" << tailRunner->currentNodeType();
            tailRunner->hopForward();
        }
    }
    if (!headRunner->atVector()) {
//        qDebug() << "eliminated deadlock at head:" << headRunner->currentNodeType();
        tailRunner->moveForward(headRunner->hopForward());
    }

    do {
        Guide wBack = tailRunner->windowBehind(range / 2, &backRem);
        Guide wMid = tailRunner->windowAhead(range / 2, &midRem);
        Guide wFwd = headRunner->windowAhead(range / 2, &fwdRem);

        b_cPt = tailRunner->point();
        f_cPt = headRunner->point();

        qreal b_underShoot = backRem / range * 2;
        QPointF b_negVec = wBack.weightedAverage(range, b_cPt);
        QPointF b_posVec = wMid.weightedAverage(range, b_cPt);
        QPointF f_negVec = wMid.reversed().weightedAverage(range, f_cPt);
        QPointF f_posVec = wFwd.weightedAverage(range, f_cPt);
        qreal b_overShoot = midRem / range * 2;

        qreal f_underShoot = b_overShoot;
        qreal f_overShoot = fwdRem / range * 2;

        b_negPt = b_negVec * (1 - b_overShoot) + b_posVec * b_overShoot;
        b_posPt = b_posVec * (1 - b_underShoot) + b_negVec * b_underShoot;
        f_negPt = f_negVec * (1 - f_overShoot) + f_posVec * f_overShoot;
        f_posPt = f_posVec * (1 - f_underShoot) + f_negVec * f_underShoot;

        QVector2D b_vec(b_posPt - b_negPt);
        QVector2D f_vec(f_posPt - f_negPt);

        backCurve = qAbs(QVector2D::dotProduct(b_vec, tailRunner->vnode()->normalF()));
        frontCurve = qAbs(QVector2D::dotProduct(f_vec, headRunner->vnode()->normalF()));

        if (tailRunner->atVector() && headRunner->atVector() && backCurve < thrValue * 0.09 && frontCurve < thrValue * 0.09) {
            qreal head_dp = headRunner->hopForward();
            qreal tail_dp = tailRunner->strictMoveForward(head_dp);
            if (head_dp > tail_dp) {
                *headRunner = *tailRunner;
                headRunner->moveForward(range / 2);
                break;
            }
        } else {
            cstep = threshold * range / (1 + backCurve / thrValue * 0.4) ;
            fstep = threshold * range / (1 + frontCurve / thrValue * 0.4) ;
            if (frontCurve == 0) {
                if (backCurve > 0)
                    rstep = cstep;
                else
                    rstep = fstep;
            } else if (backCurve == 0)
                rstep = fstep;
            else
                rstep = (cstep + fstep) * 0.5;
            qreal head_dp;
            if (headRunner->hasNext())
                head_dp = headRunner->strictMoveForward(rstep);
            else
                head_dp = rstep;
            qreal tail_dp = tailRunner->strictMoveForward(head_dp);

            if (head_dp > tail_dp && !headRunner->atPoint()) {
                *headRunner = *tailRunner;
                headRunner->moveForward(range / 2);
            }
            break;
        }
    } while (headRunner->atVector() && tailRunner->atVector());
    QPointF currentPt = tailRunner->atPoint() ? tailRunner->point() : b_cPt + b_negPt + b_posPt;
    QPointF aheadPt = headRunner->atPoint() ? headRunner->point() : f_cPt + f_negPt + f_posPt;

    if (bNegPt)
        *bNegPt = b_negPt;
    if (bPosPt)
        *bPosPt = b_posPt;
    count++;
    return QLineF(currentPt, aheadPt);
}


void SmoothRunner::beginSmooth()
{
    qreal backRem;
    qreal fwdRem;
    m_negWay = windowBehind(m_smoothRadius / 2, &backRem);
    m_posWay = windowAhead(m_smoothRadius  / 2, &fwdRem);
    qreal b_underShoot = backRem / m_smoothRadius * 2;
    qreal b_overShoot = fwdRem / m_smoothRadius * 2;
    QPointF b_cPt = point();
    QPointF b_negVec = m_negWay.weightedAverage(m_smoothRadius, b_cPt);
    QPointF b_posVec = m_posWay.weightedAverage(m_smoothRadius, b_cPt);
    m_negPt = b_negVec * (1 - b_overShoot) + b_posVec * b_overShoot;
    m_posPt = b_posVec * (1 - b_underShoot) + b_negVec * b_underShoot;
    ahead = *this;
    ahead.strictMoveForward(m_smoothRadius / 2);
}

QPointF SmoothRunner::smoothedPoint() const
{
    return atPoint() ? point() : point() + m_negPt + m_posPt;
}

qreal SmoothRunner::smoothRadius() const
{
    return m_smoothRadius;
}

void SmoothRunner::setSmoothRadius(qreal smoothRadius)
{
    m_smoothRadius = smoothRadius;
}
