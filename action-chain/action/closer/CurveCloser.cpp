ICloserGenerator curveCloserGenerator() {
    return []() {
        return new CurveCloser();
    };
}

CurveCloser::CurveCloser() : ICloser()
{
}

CurveCloser::~CurveCloser()
{
}

void CurveCloser::init()
{
}

bool CurveCloser::isClosed(PerceptionReport* report)
{
    return false;
}
