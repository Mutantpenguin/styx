#ifndef CRESOURCECACHEBASE_HPP
#define CRESOURCECACHEBASE_HPP

#include <string>
#include <memory>

class CResourceCacheBase
{
private:
	CResourceCacheBase( const CResourceCacheBase& rhs );
	CResourceCacheBase& operator = ( const CResourceCacheBase& rhs );

protected:
	CResourceCacheBase( const std::string &name );
	virtual ~CResourceCacheBase( void );

public:
	virtual void CollectGarbage( void ) = 0;
	virtual void Reload( void ) = 0;

	const std::string &Name( void );

protected:
	const std::string m_name;
};

#endif // CRESOURCECACHEBASE_HPP
