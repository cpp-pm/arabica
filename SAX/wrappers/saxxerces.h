#ifndef saxxercesH
#define saxxercesH
//---------------------------------------------------------------------------
// A SAX2 wrapper class for Xerces.
//
// $Id$
//---------------------------------------------------------------------------

#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <SAX/Attributes.h>
#include <SAX/InputSource.h>
#include <SAX/XMLReader.h>
#include <SAX/SAXParseException.h>
#include <SAX/SAXNotRecognizedException.h>
#include <SAX/SAXNotSupportedException.h>
#include <SAX/ext/LexicalHandler.h>
#include <SAX/ext/DeclHandler.h>
#include <SAX/helpers/PropertyNames.h>
#include <SAX/wrappers/XercesFeatureNames.h>

// Xerces Includes
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax2/LexicalHandler.hpp>
#include <xercesc/sax2/DeclHandler.hpp>
#include <xercesc/sax/DTDHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/sax/InputSource.hpp>
#ifdef (__GNUG__ && (__GNUC__ < 3))
#include <xercesc/util/TransService.hpp>
#endif


namespace SAX
{

template<class string_type, class string_adaptor_type = SAX::default_string_adaptor<string_type> >
class xerces_wrapper : public SAX::basic_XMLReader<string_type>
{
	public:
    typedef string_type stringT;
    typedef string_adaptor_type string_adaptorT;
    typedef SAX::basic_EntityResolver<stringT> entityResolverT;
    typedef SAX::basic_DTDHandler<stringT> dtdHandlerT;
    typedef SAX::basic_ContentHandler<stringT> contentHandlerT;
    typedef SAX::basic_LexicalHandler<stringT> lexicalHandlerT;
    typedef SAX::basic_DeclHandler<stringT> declHandlerT;
    typedef SAX::basic_InputSource<stringT> inputSourceT;
    typedef SAX::basic_Locator<stringT> locatorT;
    typedef SAX::basic_XMLReader<stringT> base;
    typedef SAX::XercesFeatureNames<stringT, string_adaptorT> featuresT;

    xerces_wrapper();
		virtual ~xerces_wrapper();

    /////////////////////////////////////////////////
    // Configuration
    virtual bool getFeature(const stringT& name) const;
    virtual void setFeature(const stringT& name, bool value);
  
    /////////////////////////////////////////////////
    // Event Handlers
    virtual void setEntityResolver(entityResolverT& resolver); 
    virtual entityResolverT* getEntityResolver() const;
    virtual void setDTDHandler(SAX::basic_DTDHandler<stringT>& handler) { dtdHandlerAdaptor_.setDTDHandler(handler); }
    virtual SAX::basic_DTDHandler<stringT>* getDTDHandler() const { return dtdHandlerAdaptor_.getDTDHandler(); }
    virtual void setContentHandler(SAX::basic_ContentHandler<stringT>& handler) { contentHandlerAdaptor_.setContentHandler(handler); }
    virtual SAX::basic_ContentHandler<stringT>* getContentHandler() const { return contentHandlerAdaptor_.getContentHandler(); }
    virtual void setErrorHandler(SAX::ErrorHandler& handler) { errorHandlerAdaptor_.setErrorHandler(handler); }
    virtual SAX::ErrorHandler* getErrorHandler() const { return errorHandlerAdaptor_.getErrorHandler(); }

    //////////////////////////////////////////////////
    // Parsing
    virtual void parse(SAX::basic_InputSource<stringT>& input);

  protected:
    virtual std::auto_ptr<typename base::PropertyBase> doGetProperty(const stringT& name);
    virtual void doSetProperty(const stringT& name, std::auto_ptr<typename base::PropertyBase> value);

  private:

    ///////////////////////////////
    // Xerces platform initializer
    /**
     * Class to handle calls to XMLPlatformUtils::Initialize() and
     * XMLPlatformUtils::Terminate().
     *
     * Without such a class, calls to Initialize() and Terminate() may be nested
     * and cause evil consequences.  Eg. A typical use of the old xerces_wrapper
     * could have resulted in:
     * <pre>
     * // Construct two xerces_wrappers.
     *     XMLPlatformUtils::Initialize()
     *     XMLPlatformUtils::Initialize()
     *     
     * // do stuff
     *
     * // Get rid of one of the xerces_wrappers
     *     XMLPlatformUtils::Terminate()
     *     
     * // do more stuff -- this is _after_ a call to Terminate()...
     *
     *     XMLPlatformUtils::Terminate()
     * </pre>
     */
    class xerces_initializer
    {
      public:
	xerces_initializer() { doInitialize(); }
	~xerces_initializer() { doTerminate(); }

	static bool isInitialized() { return count_ > 0; }

  private:
	void doInitialize()
	{
	  if (!count_)
	  {
	    XMLPlatformUtils::Initialize();
	  }
	  ++count_;
	}
	void doTerminate()
	{
	  --count_;
	  if (!count_)
	  {
	    XMLPlatformUtils::Terminate();
	  }
	}

	static int count_;
    };

    ///////////////////////////////
    // String adaptor for XMLCh

#if !(__GNUG__ && (__GNUC__ < 3))
    // * For the null-terminated version, we convert the array of XMLCh to a
    //   null-terminated vector of wchar_t.  We then pass &vector[0] to the
    //   parent method.
    // * For the known length version, we convert the the array of XMLCh to a
    //   vector of wchar_t.  We then pass &vector[0] and length to the parent
    //   method.
#else
    // * For the null-terminated version, we use the Xerces XMLTranscoder
    //   classes to convert to a null-terminated utf-8 char array.  We then
    //   pass this to the parent method.
    // * For the known length version, we copy the array, append a
    //   null-character and then do the above.
#endif
    class xerces_string_adaptor : public string_adaptorT
    {
      public:
	typedef typename stringT::value_type value_type;
	typedef string_adaptorT base;

#if !(__GNUG__ && (__GNUC__ < 3))

	typedef std::vector<wchar_t> wVector;

	stringT makeStringT(const XMLCh* str) const
	{
	  if (str)
	  {
	    wVector buffer;
	    std::insert_iterator<wVector> inserter(buffer, buffer.begin());

	    // We want the trailing 0 character.
	    *inserter = *str;
	    while (*str != 0)  // str points to the character we've just copied
	    {
	      *++inserter = *++str;
	    }
	    return base::makeStringT(&buffer[0]);
	  }
	  else
	  {
	    return base::makeStringT("");
	  }
	}

	stringT makeStringT(const XMLCh* str, int length) const
	{
	  if (str && length)
	  {
	    wVector buffer;
	    std::insert_iterator<wVector> inserter(buffer, buffer.begin());

	    // We don't want any trailing characters.
	    const XMLCh* end = str + length;
	    while (str < end)
	    {
	      *inserter++ = *str++;
	    }
	    return base::makeStringT(&buffer[0], length);
	  }
	  else
	  {
	    return base::makeStringT("");
	  }
	}
#else

	xerces_string_adaptor() : transcoder_(0) { }

	~xerces_string_adaptor() { delete transcoder_; }

	static const int MAX_BUFFER = 4096;

	/* Lazily instantiates a transcoder, but only if
	 * XMLPlatformUtils::Initialize() has been called.
	 * @see xerces_initializer */
	XMLTranscoder* getTranscoder() const
	{
	  if (!transcoder_)
	  {
	    if (xerces_initializer::isInitialized())
	    {
	      // NOTE: We are going to assume that result is always "Ok".
	      ::XMLTransService::Codes result;
	      transcoder_ = ::XMLPlatformUtils::fgTransService->makeNewTranscoderFor(XMLUni::fgUTF8EncodingString, result, MAX_BUFFER);
	    }
	  }
	  return transcoder_;
	}

	char* transcode(const XMLCh* toTranscode) const
	{
	  if (0 != getTranscoder())
	  {
	    vector<XMLByte> final;
	    vector<XMLByte> toFill(MAX_BUFFER);
	    unsigned int charsEaten = 1;
	    while (*toTranscode && charsEaten)
	    {
	      unsigned int srcLen = ::XMLString::stringLen(toTranscode);
	      // Ensure we have enough space.
	      toFill.resize(MAX_BUFFER);
	      // Resize to the number of bytes written.
	      unsigned int size = 
		getTranscoder()->transcodeTo(toTranscode,
					     srcLen,
					     &toFill[0],
					     MAX_BUFFER,
					     charsEaten,
					     ::XMLTranscoder::UnRep_RepChar);

	      toFill.resize(size);
	      final.insert(final.end(), toFill.begin(), toFill.end());
	      toTranscode += charsEaten;
	    }
	    final.push_back(0);
	    char *result = new char[final.size()];
	    memcpy(result, &final[0], final.size()*sizeof(char));
	    return result;
	  }
	  else
	  {
	    char *result = ::XMLString::transcode(toTranscode);
	    return result;
	  }
	}

	// Create a string from a zero-terminated array of XMLCh
	stringT makeStringT(const XMLCh* str) const
	{
	  if (str)
	  {
	    ::ArrayJanitor<char> buffer(transcode(str));
	    return base::makeStringT(&buffer[0]);
	  }
	  else
	  {
	    return base::makeStringT("");
	  }
	}
	// Create a string from a given number of XMLCh within an array
	stringT makeStringT(const XMLCh* str, int length) const
	{
	  if (str && length)
	  {
	    std::vector<XMLCh> nullTerminated(length+1);
	    nullTerminated.assign(str, str+length+1);
	    nullTerminated.push_back(0);
	    ::ArrayJanitor<char> buffer(transcode(&nullTerminated[0]));
	    return base::makeStringT(&buffer[0]);
	  }
	  else
	  {
	    return base::makeStringT("");
	  }
	}

#endif

	XMLCh* asXMLChString(const stringT& s) const
	{
	  std::string str = base::asStdString(s);
	  return ::XMLString::transcode(str.c_str());
	}

#if (__GNUG__ && (__GNUC__ < 3))
      private:
	mutable XMLTranscoder* transcoder_;
#endif
    };

    ///////////////////////////////
    // Adaptors
    class LocatorAdaptor : public SAX::basic_Locator<stringT>
    {
      public:
        LocatorAdaptor() : locator_(0) { }
        LocatorAdaptor(const ::Locator* const locator) : locator_(locator) { } 
        ~LocatorAdaptor() { }

        stringT getPublicId() const
        {
          if(!locator_) 
            return stringT();

          return SA_.makeStringT(locator_->getPublicId());
        } // getPublicId
    
        stringT getSystemId() const
        {
          if(!locator_)
            return stringT();

          return SA_.makeStringT(locator_->getSystemId());
        } // getSystemId
    
        int getLineNumber() const
        {
          if(!locator_)
            return -1;

          return locator_->getLineNumber();
        } // getLineNumber
    
        int getColumnNumber() const
        {
          if(!locator_)
            return -1;

          return locator_->getColumnNumber();
        } // getColumnNumber
    
        void setLocator(const ::Locator* const locator)
        {
          locator_ = locator;
        } // setLocator

      private:
        const ::Locator* locator_;
        xerces_string_adaptor SA_;
    }; // class LocatorAdaptor

    class DTDHandlerAdaptor : public ::DTDHandler 
    {
      public:
        DTDHandlerAdaptor() : dtdHandler_(0) { }
        ~DTDHandlerAdaptor() { }

        void setDTDHandler(SAX::basic_DTDHandler<stringT>& handler) { dtdHandler_ = &handler; }
        SAX::basic_DTDHandler<stringT>* getDTDHandler() const { return dtdHandler_; }

        virtual void notationDecl(const XMLCh* const name,
                                  const XMLCh* const publicId, 
                                  const XMLCh* const systemId)
        {
          if(dtdHandler_)
            dtdHandler_->notationDecl(SA_.makeStringT(name),
                                      SA_.makeStringT(publicId),
                                      SA_.makeStringT(systemId));
        } // notationDecl
        virtual void unparsedEntityDecl(const XMLCh* const name,
                                        const XMLCh* const publicId, 
                                        const XMLCh* const systemId,
                                        const XMLCh* const notationName)
        {
          if(dtdHandler_)
            dtdHandler_->unparsedEntityDecl(SA_.makeStringT(name),
                                            SA_.makeStringT(publicId),
                                            SA_.makeStringT(systemId),
                                            SA_.makeStringT(notationName));
        } // unparsedEntityDecl

        virtual void resetDocType() 
        {
          // no op
        } // resetDocType

      private:
        SAX::basic_DTDHandler<stringT>* dtdHandler_;
        xerces_string_adaptor SA_;
    }; // class DTDHandlerAdaptor

    class ContentHandlerAdaptor : public ::ContentHandler 
    {
      public:
        ContentHandlerAdaptor() : contentHandler_(0) { } 
        ~ContentHandlerAdaptor() { }

        void setContentHandler(SAX::basic_ContentHandler<stringT>& handler) { contentHandler_ = &handler; }
        SAX::basic_ContentHandler<stringT>* getContentHandler() const { return contentHandler_; }

        virtual void setDocumentLocator(const ::Locator* const locator)
        {
          locator_.setLocator(locator);
          if(contentHandler_) 
            contentHandler_->setDocumentLocator(locator_);
        } // putDocumentLocation
        
        virtual void startDocument()
        {
          if(contentHandler_) 
            contentHandler_->startDocument();
        } // startDocument
        
        virtual void endDocument()
        {
          if(contentHandler_) 
            contentHandler_->endDocument();
        } // endDocument
        
        virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri)
        {
          if(contentHandler_)
            contentHandler_->startPrefixMapping(SA_.makeStringT(prefix), SA_.makeStringT(uri));
        } // startPrefixMapping
       
        virtual void endPrefixMapping(const XMLCh* const prefix)
        {
          if(contentHandler_)
            contentHandler_->endPrefixMapping(SA_.makeStringT(prefix));
        } // endPrefixMapping
        
        virtual void startElement(const XMLCh* const namespaceUri,
                                  const XMLCh* const localName,
                                  const XMLCh* const rawName,
                                  const ::Attributes& attrs)
        {
          if(contentHandler_)
          {
            AttributesAdaptor attrAdaptor(attrs);
            contentHandler_->startElement(SA_.makeStringT(namespaceUri),
                                          SA_.makeStringT(localName),
                                          SA_.makeStringT(rawName),
                                          attrAdaptor);
          } // if ...
        } // startElement
        
        virtual void endElement(const XMLCh* const namespaceUri,
                                const XMLCh* const localName,
                                const XMLCh* const rawName)
        {
          if(contentHandler_)
            contentHandler_->endElement(SA_.makeStringT(namespaceUri),
                                        SA_.makeStringT(localName),
                                        SA_.makeStringT(rawName));
        } // endElement
        
        virtual void characters(const XMLCh* const chars, const unsigned int length)
        {
          if(contentHandler_) 
            contentHandler_->characters(SA_.makeStringT(chars, length));
        } // characters
        
        virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
        {
          if(contentHandler_)
            contentHandler_->ignorableWhitespace(SA_.makeStringT(chars, length));
        } // ignorableWhitespace
        
        virtual void processingInstruction(const XMLCh* const target,
                                           const XMLCh* const data)
        {
          if(contentHandler_)
            contentHandler_->processingInstruction(SA_.makeStringT(target),
                                                   SA_.makeStringT(data));
        } // processingInstruction
        
        virtual void skippedEntity(const XMLCh* const name)
        {
          if(contentHandler_)
            contentHandler_->skippedEntity(SA_.makeStringT(name));
        } // skippedEntity

      private:
        ////////////////////////////////////////////////
        // member varaibles
        SAX::basic_ContentHandler<stringT>* contentHandler_;
        LocatorAdaptor locator_;
        xerces_string_adaptor SA_;

        //////////////////////////////////////////////////////
        // COM interface -> C++ interface adaptors
        class AttributesAdaptor : public SAX::basic_Attributes<stringT>
        {
          public:
            AttributesAdaptor(const ::Attributes& attrs) : attributes_(attrs) { }
            ~AttributesAdaptor() { }

            /////////////////////////
            // indexed access
            virtual int getLength() const
            {
              return attributes_.getLength();
            } // getLength
            virtual stringT getURI(unsigned int index) const
            {
              return SA_.makeStringT(attributes_.getURI(index));
            } // getURI
            virtual stringT getLocalName(unsigned int index) const
            {
              return SA_.makeStringT(attributes_.getLocalName(index));
            } // getLocalName
            virtual stringT getQName(unsigned int index) const
            {
              return SA_.makeStringT(attributes_.getQName(index));
            } // getQName
            virtual stringT getType(unsigned int index) const
            {
              return SA_.makeStringT(attributes_.getType(index));
            } // getType
            virtual stringT getValue(unsigned int index) const
            {
              return SA_.makeStringT(attributes_.getValue(index));
            } // getValue

            /////////////////////////
            // name based query
            virtual int getIndex(const stringT& uri, const stringT& localName) const
            {
	      ::ArrayJanitor<XMLCh> wUri(SA_.asXMLChString(uri));
	      ::ArrayJanitor<XMLCh> wLocalName(SA_.asXMLChString(localName));
	      return attributes_.getIndex(&wUri[0], &wLocalName[0]);
            } // getIndex
            virtual int getIndex(const stringT& qName) const
            {
	      ::ArrayJanitor<XMLCh> wQName(SA_.asXMLChString(qName));
	      return attributes_.getIndex(&wQName[0]);
            } // getIndex
            virtual stringT getType(const stringT& uri, const stringT& localName) const
            {
	      ::ArrayJanitor<XMLCh> wUri(SA_.asXMLChString(uri));
	      ::ArrayJanitor<XMLCh> wLocalName(SA_.asXMLChString(localName));
	      return SA_.makeStringT(attributes_.getType(&wUri[0], &wLocalName[0]));
            } // getType
            virtual stringT getType(const stringT& qName) const
            {
	      ::ArrayJanitor<XMLCh> wQName(SA_.asXMLChString(qName));
	      return SA_.makeStringT(attributes_.getType(&wQName[0]));
            } // getType
            virtual stringT getValue(const stringT& uri, const stringT& localName) const
            {
	      ::ArrayJanitor<XMLCh> wUri(SA_.asXMLChString(uri));
	      ::ArrayJanitor<XMLCh> wLocalName(SA_.asXMLChString(localName));
	      return SA_.makeStringT(attributes_.getValue(&wUri[0], &wLocalName[0]));
            } // getValue
            virtual stringT getValue(const stringT& qname) const
            {
	      ::ArrayJanitor<XMLCh> wQName(SA_.asXMLChString(qname));
	      return SA_.makeStringT(attributes_.getValue(&wQName[0]));
            } // getValue

          private:
            const ::Attributes& attributes_;
	    xerces_string_adaptor SA_;

            AttributesAdaptor();
        }; // class AttributesAdaptor
    }; // class ContentHandlerAdaptor

    class ErrorHandlerAdaptor : public ::ErrorHandler 
    {
      public:
        ErrorHandlerAdaptor() :   errorHandler_(0) { }
        virtual ~ErrorHandlerAdaptor() { }

        void setErrorHandler(SAX::ErrorHandler& handler) { errorHandler_ = &handler; }
        SAX::ErrorHandler* getErrorHandler() const { return errorHandler_; }

        virtual void warning(const ::SAXParseException& exception)
        {
          handleError(exception, &SAX::ErrorHandler::warning);
        } // warning

        virtual void error(const ::SAXParseException& exception)
        {
          handleError(exception, &SAX::ErrorHandler::error);
        } // error

        virtual void fatalError(const ::SAXParseException& exception)
        {
          handleError(exception, &SAX::ErrorHandler::fatalError);
        } // fatalError

        virtual void resetErrors() 
        { 
          // no op 
        } // resetErrors

      private:
        typedef void(SAX::ErrorHandler::* ErrorFn)(const SAX::SAXParseException&);

        void handleError(const ::SAXParseException& exception, ErrorFn fn)

        {
          if(!errorHandler_)
            return;

          stringT errorMsg(SA_.makeStringT(exception.getMessage()));
          SAX::basic_SAXParseException<stringT> sp(SA_.asStdString(errorMsg),
                                                   SA_.makeStringT(exception.getPublicId()),
                                                   SA_.makeStringT(exception.getSystemId()),
                                                   exception.getLineNumber(),
                                                   exception.getColumnNumber());
          (errorHandler_->*fn)(sp);
        } // handleError

        SAX::ErrorHandler* errorHandler_;
        xerces_string_adaptor SA_;
    }; // class ErrorHandlerAdaptor

    class LexicalHandlerAdaptor : public ::LexicalHandler 
    {
      public:
        LexicalHandlerAdaptor() : lexicalHandler_(0) { }
        virtual ~LexicalHandlerAdaptor() { }

        void setLexicalHandler(lexicalHandlerT& handler) { lexicalHandler_ = &handler; }
        lexicalHandlerT* getLexicalHandler() const { return lexicalHandler_; }

        virtual void startDTD(const XMLCh* const name,
                              const XMLCh* const publicId,
                              const XMLCh* const systemId)
        {
          if(lexicalHandler_)
            lexicalHandler_->startDTD(SA_.makeStringT(name),
                                      SA_.makeStringT(publicId),
                                      SA_.makeStringT(systemId));
        } // startDTD

        virtual void endDTD()
        {
          if(lexicalHandler_)
            lexicalHandler_->endDTD();
        } // endDTD

        virtual void startEntity(const XMLCh* const name)
        {
          if(lexicalHandler_)
            lexicalHandler_->startEntity(SA_.makeStringT(name));
        } // startEntity

        virtual void endEntity(const XMLCh* const name)
        {
          if(lexicalHandler_)
            lexicalHandler_->endEntity(SA_.makeStringT(name));
        } // endEntity

        virtual void startCDATA()
        {
          if(lexicalHandler_)
            lexicalHandler_->startCDATA();
        } // startCDATA

        virtual void endCDATA()
        {
          if(lexicalHandler_)
            lexicalHandler_->endCDATA();
        } // endCDATA

        virtual void comment(const XMLCh* const chars, const unsigned int length)
        {
          if(lexicalHandler_)
            lexicalHandler_->comment(SA_.makeStringT(chars, length));
        } // comment

      private:
        lexicalHandlerT* lexicalHandler_;
        xerces_string_adaptor SA_;
    }; // class LexicalHandlerAdaptor

    class DeclHandlerAdaptor : public ::DeclHandler 
    {
      public:
        DeclHandlerAdaptor() : declHandler_(0) { }
        virtual ~DeclHandlerAdaptor() { }

        void setDeclHandler(SAX::basic_DeclHandler<stringT>& handler) { declHandler_ = &handler; }
        SAX::basic_DeclHandler<stringT>* getDeclHandler() const { return declHandler_; }

        virtual void elementDecl(const XMLCh* const name,
                                 const XMLCh* const model)
        {
          if(declHandler_)
            declHandler_->elementDecl(SA_.makeStringT(name),
                                      SA_.makeStringT(model));
        } // elementDecl

        virtual void attributeDecl(const XMLCh* const elementName,
                                   const XMLCh* const attributeName,
                                   const XMLCh* const type,
                                   const XMLCh* const valueDefault,
                                   const XMLCh* const value)
        {
          if(declHandler_)
            declHandler_->attributeDecl(SA_.makeStringT(elementName),
                                        SA_.makeStringT(attributeName),
                                        SA_.makeStringT(type),
                                        SA_.makeStringT(valueDefault),
                                        SA_.makeStringT(value));
        } // attributeDecl

        virtual void internalEntityDecl(const XMLCh* const name,
                                        const XMLCh* const value)
        {
          if(declHandler_)
            declHandler_->internalEntityDecl(SA_.makeStringT(name),
                                             SA_.makeStringT(value));
        } // internalEntityDecl

        virtual void externalEntityDecl(const XMLCh* const name,
                                        const XMLCh* const publicId,
                                        const XMLCh* const systemId)
        {
          if(declHandler_)
            declHandler_->externalEntityDecl(SA_.makeStringT(name),
                                             SA_.makeStringT(publicId),
                                             SA_.makeStringT(systemId));
        } // externalEntityDecl

      private:
        SAX::basic_DeclHandler<stringT>* declHandler_;
        xerces_string_adaptor SA_;
    }; // class DeclHandlerAdaptor

    class IStreamAdaptor : public ::BinInputStream
    {
      public:
        IStreamAdaptor(std::istream* istream) : istream_(istream), curPos_(0) { }
        virtual ~IStreamAdaptor() { }

        virtual unsigned int curPos() const
        {
          return curPos_;
        } // curPos

        virtual unsigned int readBytes(XMLByte* const toFill, const unsigned int maxToRead)
        {
          istream_->read(reinterpret_cast<char*>(toFill), maxToRead);
          curPos_ += istream_->gcount();
          return istream_->gcount();
        } // readBytes

      private:
        std::istream* istream_;
        unsigned int curPos_;
    }; // IStreamAdaptor

    class InputSourceAdaptor : public ::InputSource
    {
      public:
        InputSourceAdaptor(SAX::basic_InputSource<stringT>& source) : inputSource_(source) 
        { 
          setPublicId(SA_.asXMLChString(inputSource_.getPublicId()));
          setSystemId(SA_.asXMLChString(inputSource_.getSystemId()));
        } // InputSourceAdaptor
        virtual ~InputSourceAdaptor() { }

        virtual BinInputStream* makeStream() const
        {
          return new IStreamAdaptor(inputSource_.getByteStream());
        } // makeStream
     
      private:
        SAX::basic_InputSource<stringT>& inputSource_;
        xerces_string_adaptor SA_;
    }; // class InputSourceWrapper


    /////////////////////////////////////////////
    // Member variables
    std::auto_ptr<xerces_initializer> initializer_;
    ::SAX2XMLReader* xerces_;
    ContentHandlerAdaptor contentHandlerAdaptor_;
    DTDHandlerAdaptor dtdHandlerAdaptor_;
    ErrorHandlerAdaptor errorHandlerAdaptor_;
    LexicalHandlerAdaptor lexicalHandlerAdaptor_;
    DeclHandlerAdaptor declHandlerAdaptor_;
    xerces_string_adaptor SA_;
    SAX::PropertyNames<stringT, string_adaptorT> properties_;
}; // class xerces_wrapper

template<class stringT, class string_adaptorT>
xerces_wrapper<stringT, string_adaptorT>::xerces_wrapper()
{
  try
  {
    std::auto_ptr<xerces_initializer> init(new xerces_initializer);
    initializer_ = init;
  }
  catch(const XMLException& toCatch)
  {
    stringT s = SA_.makeStringT(toCatch.getMessage());
    throw SAX::SAXException(SA_.asStdString(s));
  } // catch

  xerces_ = XMLReaderFactory::createXMLReader();

  xerces_->setContentHandler(&contentHandlerAdaptor_);
  xerces_->setDTDHandler(&dtdHandlerAdaptor_);
  xerces_->setErrorHandler(&errorHandlerAdaptor_);
  xerces_->setLexicalHandler(&lexicalHandlerAdaptor_);
  xerces_->setDeclarationHandler(&declHandlerAdaptor_);
} // xerces_wrapper

template<class stringT, class string_adaptorT>
xerces_wrapper<stringT, string_adaptorT>::~xerces_wrapper()
{
  delete xerces_;
} // ~xerces_wrapper

template<class stringT, class string_adaptorT>
bool xerces_wrapper<stringT, string_adaptorT>::getFeature(const stringT& name) const
{
  try 
  {
    return xerces_->getFeature(SA_.asXMLChString(name));
  } // try
  catch(::SAXNotSupportedException& e) 
  {
    throw SAX::SAXNotSupportedException(SA_.makeStringT(e.getMessage()));
  } // catch(SAXNotSupportedException& e)
  catch(::SAXNotRecognizedException& e) 
  {
    throw SAX::SAXNotRecognizedException(SA_.makeStringT(e.getMessage()));
  } // catch(SAXNotRecognizedException& e)
} // getFeature

template<class stringT, class string_adaptorT>
void xerces_wrapper<stringT, string_adaptorT>::setFeature(const stringT& name, bool value)
{
  try 
  {
    xerces_->setFeature(SA_.asXMLChString(name), value);
  } // try
  catch(::SAXNotSupportedException& e) 
  {
    throw SAX::SAXNotSupportedException(SA_.makeStringT(e.getMessage()));
  } // catch(SAXNotSupportedException& e)
  catch(::SAXNotRecognizedException& e) 
  {
    throw SAX::SAXNotRecognizedException(SA_.makeStringT(e.getMessage()));
  } // catch(SAXNotRecognizedException& e)
} // setFeature

template<class stringT, class string_adaptorT>
void xerces_wrapper<stringT, string_adaptorT>::setEntityResolver(entityResolverT& resolver) 
{
} // setEntityResolver

template<class stringT, class string_adaptorT>
SAX::basic_EntityResolver<stringT>* xerces_wrapper<stringT, string_adaptorT>::getEntityResolver() const
{
  return 0;
} // getEntityResolver

template<class stringT, class string_adaptorT>
std::auto_ptr<typename SAX::basic_XMLReader<stringT>::PropertyBase> xerces_wrapper<stringT, string_adaptorT>::doGetProperty(const stringT& name)
{
  if(name == properties_.lexicalHandler)
  {
    SAX::basic_XMLReader<stringT>::Property<lexicalHandlerT *> *prop = new SAX::basic_XMLReader<stringT>::Property<lexicalHandlerT *>(lexicalHandlerAdaptor_.getLexicalHandler());
    return std::auto_ptr<typename SAX::basic_XMLReader<stringT>::PropertyBase>(prop);
  }
  if(name == properties_.declHandler)
  {
    SAX::basic_XMLReader<stringT>::Property<SAX::basic_DeclHandler<stringT>*>* prop = new SAX::basic_XMLReader<stringT>::Property<SAX::basic_DeclHandler<stringT>*>(declHandlerAdaptor_.getDeclHandler());
    return std::auto_ptr<typename SAX::basic_XMLReader<stringT>::PropertyBase>(prop);
  }
  throw SAX::SAXNotRecognizedException("Property not recognized ");    
} // doGetProperty

template<class stringT, class string_adaptorT>
void xerces_wrapper<stringT, string_adaptorT>::doSetProperty(const stringT& name, std::auto_ptr<typename base::PropertyBase> value)
{
  if(name == properties_.lexicalHandler)
  {
    SAX::basic_XMLReader<stringT>::Property<lexicalHandlerT&>* prop = dynamic_cast<SAX::basic_XMLReader<stringT>::Property<lexicalHandlerT&>*>(value.get());

    if(!prop)
#if (__GNUG__)
#if (__GNUC__ < 3)
      throw std::runtime_error("Property LexicalHandler is wrong type, should be SAX::LexicalHandler&");
#else // (__GNUC__ < 3)
      throw std::bad_cast();
#endif // (__GNUC__ < 3)
#else (__GNUG__)
      throw std::bad_cast("Property LexicalHandler is wrong type, should be SAX::LexicalHandler&");
#endif (__GNUG__)

    lexicalHandlerAdaptor_.setLexicalHandler(prop->get());
    return;
  } // if ...
  if(name == properties_.declHandler)
  {
    SAX::basic_XMLReader<stringT>::Property<SAX::basic_DeclHandler<stringT>&>* prop = dynamic_cast<SAX::basic_XMLReader<stringT>::Property<SAX::basic_DeclHandler<stringT>&>*>(value.get());

    if(!prop)
#if (__GNUG__)
#if (__GNUC__ < 3)
      throw std::runtime_error("Property DeclHandler is wrong type, should be SAX::DeclHandler&");
#else // (__GNUC__ < 3)
      throw std::bad_cast();
#endif // (__GNUC__ < 3)
#else (__GNUG__)
      throw std::bad_cast("Property DeclHandler is wrong type, should be SAX::DeclHandler&");
#endif (__GNUG__)
    declHandlerAdaptor_.setDeclHandler(prop->get());
    return;
  } // if ...
  throw SAX::SAXNotRecognizedException("Property not recognized ");    
} // doSetProperty

template<class stringT, class string_adaptorT>
void xerces_wrapper<stringT, string_adaptorT>::parse(SAX::basic_InputSource<stringT>& source)
{
  // if no stream is open, let Xerces deal with it
  if(source.getByteStream() == 0)
    xerces_->parse(SA_.asStdString(source.getSystemId()).c_str()); 
  else
  {
    // we have to wrap everything up in a Xerces friendly way
    InputSourceAdaptor isAdaptor(source);
    xerces_->parse(isAdaptor);
  } // if ...
} // parse

template<class stringT, class string_adaptorT>
int xerces_wrapper<stringT, string_adaptorT>::xerces_initializer::count_ = 0;

} // namespace SAX

#endif
// end of file
