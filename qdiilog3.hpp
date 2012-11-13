#ifndef QLOG_HPP
#define QLOG_HPP

#include <ostream>
#include <string>

// let the user defines his own namespace
#ifndef QLOG_NAMESPACE
#   define QLOG_NAMESPACE qlog
#endif

namespace QLOG_NAMESPACE
{
// -------------------------------------------------------------------------- //
// the different levels of logging

namespace loglevel
{
    static const unsigned disabled = 0;

    static const unsigned debug = 1;
    static const unsigned trace = 2;
    static const unsigned info = 3;
    static const unsigned warning = 4;
    static const unsigned error = 5;
}

// -------------------------------------------------------------------------- //
/**@struct logger
 * @brief An object that logs messages */
template< unsigned level >
struct logger
{
    explicit
    logger( bool _disabled = false )
        :m_disabled( _disabled )
        ,m_output( 0 )
        ,m_prepend( 0 )
        ,m_append( 0 )
    {}

    bool isDisabled() const { return m_disabled; }

    template< typename T >
    void treat( const T & _message, bool _firstPart ) const
    {
        if (!m_disabled && m_output)
        {
            if (_firstPart && m_prepend)
                (*m_output) << m_prepend;

            (*m_output) << _message;
        }
    }

    void setOutput( std::ostream & _output )
    {
        m_output = &_output;
    }

    void append( const char * _txt )
    {
        m_append = _txt;
    }

    void prepend( const char * _txt )
    {
        m_prepend = _txt;
    }

    void signal_end() const
    {
        if (!m_disabled && m_output)
        {
            (*m_output) << m_append;
        }
    }

private:
    bool m_disabled;
    mutable std::ostream * m_output;
    const char * m_prepend;
    const char * m_append;
};

// -------------------------------------------------------------------------- //
template< unsigned level >
struct receiver
{
    explicit
    receiver( const logger<level> * _logger, bool _muted = false )
        :m_treated( false )
        ,m_logger( _logger )
        ,m_muted( _muted )
    {}

    receiver( const receiver & _copy)
        :m_treated( false )
        ,m_logger( _copy.m_logger )
        ,m_muted( _copy.m_muted )
    {
    }

    ~receiver()
    {
        if( !m_treated )
            m_logger->signal_end();
    }

    template< typename T >
    receiver treat( const T & _message, bool _firstPart ) const
    {
        m_treated = true;

        if(!m_muted)
            m_logger->treat( _message, _firstPart );

        return *this;
    }
    private:
    receiver operator=(const receiver&);

private:
    mutable bool m_treated;
    const logger<level> * m_logger;
    mutable bool m_muted;
};

// -------------------------------------------------------------------------- //
template< unsigned level,typename T > inline
receiver<level> operator<<( const receiver<level> & _receiver,  T & _message )
{
    return _receiver.treat( _message, false );
}

// -------------------------------------------------------------------------- //
template< unsigned level, typename T > inline
receiver<level> operator << ( const logger<level> & _logger, T & _message )
{
    return receiver<level>( &_logger ).treat( _message, true );
}
} // namespace

#endif QLOG_HPP
