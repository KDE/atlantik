class AtlantikProtocol : public KIO::SlaveBase
{
public:
	AtlantikProtocol( const QCString &pool, const QCString &app) : SlaveBase( "atlantik", pool, app ) {}
	virtual void get( const KURL& url );
};
