#include "OnRightEdgeCloser.h"

ICloserGenerator onRightEdgeCloserGenerator() {
    return []() {
        return new OnRightEdgeCloser();
    };
}

OnRightEdgeCloser::OnRightEdgeCloser() : ICloser() 
{
    mask = (
        PERCEPTION_REPORT_MASK_IMAGE |
        PERCEPTION_REPORT_MASK_COLOR
    );
}

OnRightEdgeCloser::~OnRightEdgeCloser()
{
}

void OnRightEdgeCloser::init()
{
}

bool OnRightEdgeCloser::isClosed() {
    PerceptionReport report;
    PerceptionReporter::getInstance().update(device, 100, mask);
    report = PerceptionReporter::getInstance().getLatest();

    if (report.isImageUpdated) {
        return false;
    }

    return true;
}