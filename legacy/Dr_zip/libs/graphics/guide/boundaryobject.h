#ifndef BOUNDARYOBJECT_H
#define BOUNDARYOBJECT_H

#include "guide.h"

#include "graphicsobject.h"
#include <guideview.h>
class GuideView;
class BoundaryObject : public GraphicsObject
{
    Q_OBJECT
    Q_ENUMS(LinkType)
    Q_ENUMS(FeedPass)
    Q_ENUMS(Operation)

    Q_PROPERTY(GuideView* prevSection READ prevSection WRITE setPrevSection NOTIFY prevSectionChanged)
    Q_PROPERTY(GuideView* nextSection READ nextSection WRITE setNextSection NOTIFY nextSectionChanged)

    Q_PROPERTY(int uidFrom READ uidFrom WRITE setUidFrom NOTIFY uidFromChanged)
    Q_PROPERTY(int uidTo READ uidTo WRITE setUidTo NOTIFY uidToChanged)

    Q_PROPERTY(Segment input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(Segment output READ output WRITE setOutput NOTIFY outputChanged)

    Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)

    Q_PROPERTY(Guide negInput READ negInput WRITE setNegInput NOTIFY negInputChanged)
    Q_PROPERTY(Guide negOutput READ negOutput WRITE setNegOutput NOTIFY negOutputChanged)

    Q_PROPERTY(Guide posInput READ posInput WRITE setPosInput NOTIFY posInputChanged)
    Q_PROPERTY(Guide posOutput READ posOutput WRITE setPosOutput NOTIFY posOutputChanged)

    Q_PROPERTY(LinkType negLink READ negLink WRITE setNegLink NOTIFY negLinkChanged)
    Q_PROPERTY(LinkType posLink READ posLink WRITE setPosLink NOTIFY posLinkChanged)

    Q_PROPERTY(Operation negOperation READ negOperation WRITE setNegOperation NOTIFY negOperationChanged)
    Q_PROPERTY(Operation posOperation READ posOperation WRITE setPosOperation NOTIFY posOperationChanged)

    Q_PROPERTY(FeedPass negFeedPass READ negFeedPass WRITE setNegFeedPass NOTIFY negFeedPassChanged)
    Q_PROPERTY(FeedPass posFeedPass READ posFeedPass WRITE setPosFeedPass NOTIFY posFeedPassChanged)

    Q_PROPERTY(qreal iHeight READ iHeight WRITE setIHeight)
    Q_PROPERTY(qreal iGap READ iGap WRITE setIGap)

    Runner m_pr;// = m_nextSection->segment(m_uidTo).posResult().headRunner();
    Runner m_nr;// = m_nextSection->segment(m_uidTo).negResult().headRunner();
    bool m_ok{false};
    QPointF m_pp;
    qreal m_pa;
    QPointF m_np;
    qreal m_na;


public:
    explicit BoundaryObject(QGraphicsItem *parent = 0);
    enum LinkType {
        NoBreaker,
        OpenBreaker,
        ClosedBreaker
    };

    enum FeedPass {
        NoPass,
        In2Out,
        Out2In
    };

    enum Operation {
        OperationCompleted,
        OperationInProgress
    };

    GuideView* prevSection() const;
    GuideView* nextSection() const;
    Guide negInput() const;
    Guide negOutput() const;
    Guide posInput() const;
    Guide posOutput() const;
    LinkType negLink() const;
    LinkType posLink() const;
    FeedPass negFeedPass() const;
    FeedPass posFeedPass() const;

    void setPrevSection(GuideView* prevSection);
    void setNextSection(GuideView* nextSection);
    void setNegInput(Guide negInput);
    void setNegOutput(Guide negOutput);
    void setPosInput(Guide posInput);
    void setPosOutput(Guide posOutput);
    void setNegLink(LinkType negLink);
    void setPosLink(LinkType posLink);
    void setNegFeedPass(FeedPass negFeedPass);
    void setPosFeedPass(FeedPass posFeedPass);

signals:
    void prevSectionChanged(GuideView* prevSection);
    void nextSectionChanged(GuideView* nextSection);
    void negInputChanged(Guide negInput);
    void negOutputChanged(Guide negOutput);
    void posInputChanged(Guide posInput);
    void posOutputChanged(Guide posOutput);
    void negLinkChanged(LinkType negLink);
    void posLinkChanged(LinkType posLink);
    void negFeedPassChanged(FeedPass negFeedPass);
    void posFeedPassChanged(FeedPass posFeedPass);
    void inputChanged(Segment input);
    void outputChanged(Segment output);
    void scaleFactorChanged(qreal scaleFactor);
    void uidFromChanged(int uidFrom);

    void uidToChanged(int uidTo);

    void negOperationChanged(Operation negOperation);

    void posOperationChanged(Operation posOperation);

public slots:
    void decoratePrev();
    void decorateNext();

    void setIHeight(qreal iHeight)
    {
        m_iHeight = iHeight;
    }

    void setIGap(qreal iGap)
    {
        m_iGap = iGap;
    }

    void setInput(Segment input)
    {
//        if (m_input == input)
//            return;

        m_input = input;
        emit inputChanged(input);
    }

    void setOutput(Segment output)
    {
    //    if (m_output == output)
    //    return;

    m_output = output;
    emit outputChanged(output);
    }

    void setScaleFactor(qreal scaleFactor)
    {
        if (m_scaleFactor == scaleFactor)
            return;

        m_scaleFactor = scaleFactor;
        emit scaleFactorChanged(scaleFactor);
    }

    void setUidFrom(int uidFrom)
    {
        if (m_uidFrom == uidFrom)
            return;

        m_uidFrom = uidFrom;
        emit uidFromChanged(uidFrom);
    }

    void setUidTo(int uidTo)
{
    if (m_uidTo == uidTo)
    return;

m_uidTo = uidTo;
emit uidToChanged(uidTo);
}

    void setNegOperation(BoundaryObject::Operation negOperation)
    {
        if (m_negOperation == negOperation)
            return;

        m_negOperation = negOperation;
        emit negOperationChanged(negOperation);
    }

    void setPosOperation(BoundaryObject::Operation posOperation)
{
    if (m_posOperation == posOperation)
    return;

m_posOperation = posOperation;
emit posOperationChanged(posOperation);
}

private:
    GuideView *m_prevSection = nullptr;
    GuideView *m_nextSection = nullptr;

    Guide m_negInput;
    Guide m_negOutput;
    Guide m_posInput;
    Guide m_posOutput;

    LinkType m_negLink;
    LinkType m_posLink;
    FeedPass m_negFeedPass;
    FeedPass m_posFeedPass;

    // QGraphicsItem interface
    qreal m_iHeight;

    qreal m_iGap;

    Segment m_input;

    Segment m_output;

    qreal m_scaleFactor;

    int m_uidFrom;

    int m_uidTo;

    Operation m_negOperation;

    Operation m_posOperation;

public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void paintTramBreaker(QPainter *painter);
    void paintTrolleyBreaker(QPainter *painter);
    void paintTramLikeBreaker(QPainter *painter, GPen penI, GPen penO, LinkType linkType);
    qreal iHeight() const
    {
        return m_iHeight;
    }
    qreal iGap() const
    {
        return m_iGap;
    }
    Segment input() const
    {
        return m_input;
    }
    Segment output() const
    {
        return m_output;
    }
    qreal scaleFactor() const
    {
        return m_scaleFactor;
    }
    int uidFrom() const
    {
        return m_uidFrom;
    }
    int uidTo() const
    {
        return m_uidTo;
    }

    // QGraphicsItem interface
public:
    QPainterPath shape() const;
    Operation negOperation() const
    {
        return m_negOperation;
    }
    Operation posOperation() const
    {
        return m_posOperation;
    }
};

#endif // BOUNDARYOBJECT_H
