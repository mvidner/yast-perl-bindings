/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:	      YPerl.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef YPerl_h
#define YPerl_h

#include <ycp/YCPList.h>


// Cheat g++ to accept a forward declaration of PerlInterpreter.
// g++ doesn't accept "class PerlInterpreter" if that "PerlInterpreter" is
// later typedef'ed to a struct.
class interpreter;
typedef struct interpreter PerlInterpreter;

class STRUCT_SV;
typedef struct STRUCT_SV SV;

class av;
typedef struct av AV;

class hv;
typedef struct hv HV;


class YPerl
{
public:

    /**
     * Parse a Perl script.
     *
     * Returns a YCPError on failure, YCPVoid on success.
     **/
    static YCPValue parse( YCPList argList );

    /**
     * Call a Perl function (sub) that doesn't return a value.
     *
     * Returns YCPVoid.
     **/
    static YCPValue callVoid( YCPList argList );
    
    /**
     * Call a Perl function (sub) that returns multiple values (a list).
     *
     * Returns the sub's result as a YCPList.
     **/
    static YCPValue callList( YCPList argList );
    
    /**
     * Call a Perl function (sub) that returns a bool value.
     *
     * Returns the sub's result as a YCPBool.
     **/
    static YCPValue callBool( YCPList argList );
    
    /**
     * Call a Perl function (sub) that returns a string value.
     *
     * Returns the sub's result as a YCPString.
     **/
    static YCPValue callString( YCPList argList );
    
    /**
     * Call a Perl function (sub) that returns an integer value.
     *
     * Returns the sub's result as a YCPInteger.
     **/
    static YCPValue callInt( YCPList argList );

    /**
     * Evaluate a Perl expression.
     *
     * Returns the expression's result in whatever type Perl returned.
     *
     * Remember that Perl tends to be somewhat surprising in what it returns;
     * the return value of a search-and-replace assignment is 'true' (1) if
     * the assignment was successful (which it usually is), not the replaced
     * text. If in doubt, evaluate the expression you really want once more.
     *
     * Example:
     *
     *     Perl::Eval( "$x=\"Hello, World!\"; $x =~ s/Hello/Good Bye/; $x" );
     *
     * The final "$x" makes the expression return the contents of $x.
     * If you omit that, you'll get the return value of the assignment
     * ( "$x =~ s/..." ) which is probably not what you want.
     *
     * Beware. Perl can be strange. - But then, you wanted it ;-)
     **/
    static YCPValue eval( YCPList argList );

    /**
     * Run the script previously loaded with parse().
     * 'argList' must be empty.
     *
     * Returns a YCPError on failure, YCPVoid on success.
     **/
    static YCPValue run( YCPList argList );

    /**
     * Load a Perl module - equivalent to "use" in Perl.
     *
     * Returns a YCPError on failure, YCPVoid on success.
     **/
    static YCPValue loadModule( YCPList argList );

    /**
     * Access the static (singleton) YPerl object. Create it if it isn't
     * created yet.
     *
     * Returns 0 on error.
     **/
    static YPerl * yPerl();

    /**
     * Access the static (singleton) YPerl object's embedde Perl
     * interpreter. Create and initialize it if it isn't created yet.
     *
     * Returns 0 on error.
     **/
    static PerlInterpreter * perlInterpreter();

    /**
     * Destroy the static (singleton) YPerl object and unload the embedded Perl
     * interpreter.
     *
     * Returns YCPVoid().
     **/
    static YCPValue destroy();

    
protected:

    /**
     * Protected constructor. Use one of the static methods rather than
     * instantiate an object of this class yourself.
     **/
    YPerl();

    /**
     * Destructor.
     **/
    ~YPerl();

    /**
     * Returns the internal embedded Perl interpreter.
     **/
    PerlInterpreter * internalPerlInterpreter() const
	{ return _perlInterpreter; }

    /**
     * Returns 'true' if the perl interpreter does have a parse tree, i.e. if
     * parse() was called previously.
     **/
    bool haveParseTree() const { return _haveParseTree; }

    /**
     * Indicate whether or not the perl interpreter does have a parse tree.
     **/
    void setHaveParseTree( bool have ) { _haveParseTree = have; }

    /**
     * Generic Perl call.
     **/
    YCPValue call( YCPList argList, YCPValueType wanted_result_type = YT_UNDEFINED );
    
    /**
     * Create a new Perl scalar value from a YCP value.
     **/
    SV * newPerlScalar( const YCPValue & val );

    /**
     * Create a Reference to a new Perl array from a YCP list.
     **/
    SV * newPerlArrayRef( const YCPList & list );

    /**
     * Create a Reference to a new Perl hash from a YCP map.
     **/
    SV * newPerlHashRef( const YCPMap & map );

    /**
     * Convert a Perl scalar to a YCPValue.
     *
     * If 'wanted_type' is something else than YT_UNDEFINED, that type is
     * forced. If the types mismatch, YCPVoid (nil) is returned and an error to
     * the log file is issued.
     **/
    YCPValue fromPerlScalar( SV * perl_scalar,
			     YCPValueType wanted_type = YT_UNDEFINED );

    /**
     * Convert a Perl array to a YCPList.
     **/
    YCPList fromPerlArray( AV * av );
    
    /**
     * Convert a Perl hash to a YCPMap.
     **/
    YCPMap fromPerlHash( HV * hv );


    // Data members.

    PerlInterpreter *	_perlInterpreter;
    bool		_haveParseTree;

    static YPerl *	_yPerl;
};


#endif	// YPerl_h