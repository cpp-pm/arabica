#ifndef ARABICA_XSLT_STYLESHEETCONSTANT_HPP
#define ARABICA_XSLT_STYLESHEETCONSTANT_HPP

#include <XML/XMLCharacterClasses.hpp>
#include <XPath/impl/xpath_namespace_context.hpp>
#include <memory>

#include "xslt_stylesheet_parser.hpp"
#include "xslt_compiled_stylesheet.hpp"
#include "xslt_compilation_context.hpp"
#include "handler/xslt_value_validation.hpp"
#include "handler/xslt_template_handler.hpp"
#include "handler/xslt_include_handler.hpp"
#include "handler/xslt_output_handler.hpp"
#include "handler/xslt_namespace_alias_handler.hpp"
#include "handler/xslt_key_handler.hpp"
#include "handler/xslt_foreign_element_handler.hpp"

namespace Arabica
{
namespace XSLT
{

template<class string_type, class string_adaptor>
class StylesheetHandler : public SAX::DefaultHandler<string_type, string_adaptor>
{
  typedef StylesheetConstant<string_type, string_adaptor> SC;
public:
  typedef string_type stringT;
  typedef string_adaptor string_adaptorT;
  typedef SAX::Attributes<string_type, string_adaptor> AttributesT;
  typedef SAX::DefaultHandler<string_type, string_adaptor> DefaultHandlerT;
  typedef CompilationContext<string_type, string_adaptor> CompilationContextT;

  StylesheetHandler(CompilationContextT& context) :
    context_(context),
    top_(false)
  {
    context_.root(*this);
    includer_.context(context_, this);      
  } // StylesheetHandler

  virtual void startDocument()
  {
    top_ = true;
  } // startDocument

  virtual void startElement(const string_type& namespaceURI,
                            const string_type& localName,
                            const string_type& qName,
                            const AttributesT& atts)
  {
    if(top_)
    {
      top_ = false;
      if(namespaceURI == SC::NamespaceURI)
        startStylesheet(namespaceURI, localName, qName, atts);
      else
        startLREAsStylesheet(namespaceURI, localName, qName, atts);
      return;
    } // if(top_)

    if(namespaceURI == SC::NamespaceURI)
      startXSLTElement(namespaceURI, localName, qName, atts);
    else if(!namespaceURI.empty())
      startForeignElement(namespaceURI, localName, qName, atts);
    else
      oops(qName);
  } // startElement

  virtual void endElement(const string_type& /* namespaceURI */,
                          const string_type& /* localName */,
                          const string_type& /* qName */)
  {
  } // endElement

  virtual void characters(const string_type& ch)
  {
    verifyNoCharacterData<string_type, string_adaptor>(ch, SC::stylesheet);
  } // characters

  virtual void endDocument()
  {
    includer_.unwind_imports();
    context_.stylesheet().prepare();
  } // endDocument

private:
  void startStylesheet(const string_type& /* namespaceURI */,
                       const string_type& localName,
                       const string_type& qName,
                       const AttributesT& atts)
  {
    if(localName != SC::stylesheet && localName != SC::transform)
      throw SAX::SAXException("Top-level element must be 'stylesheet' or 'transform'.");
    
    static const ValueRule<string_type> rules[] = { { SC::version, true },
                                                    { SC::extension_element_prefixes, false },
                                                    { SC::exclude_result_prefixes, false },
                                                    { SC::id, false },
                                                    { string_adaptor::empty_string(), false } };
    std::map<string_type, string_type> attributes = gatherAttributes(qName, atts, rules);
    if(attributes[SC::version] != SC::Version)
      throw SAX::SAXException("I'm only a poor version 1.0 XSLT Transformer.");
    if(!attributes[SC::extension_element_prefixes].empty())
      throw SAX::SAXException("Haven't implemented extension-element-prefixes yet");
  } // startStylesheet

  void startLREAsStylesheet(const string_type& namespaceURI,
                            const string_type& localName,
                            const string_type& qName,
                            const AttributesT& atts)
  {
    string_type version;
    for(int a = 0; a != atts.getLength(); ++a)
      if((SC::NamespaceURI == atts.getURI(a)) &&
         (SC::version == atts.getLocalName(a)))
      {
        version = atts.getValue(a);
        break;
      }

    if(version.empty())
      throw SAX::SAXException("The source file does not look like a stylesheet.");
    if(version != SC::Version)
      throw SAX::SAXException("I'm only a poor version 1.0 XSLT Transformer.");

    Template<string_type, string_adaptor>* lreStylesheet = 
            new Template<string_type, string_adaptor>(context_.xpath_match(SC::root_xpath), 
                                                      string_adaptor::empty_string(), 
                                                      string_adaptor::empty_string(), 
                                                      string_adaptor::empty_string(), 
                                                      context_.precedence());
    context_.push(lreStylesheet,
                  new LREStylesheetHandler<string_type, string_adaptor>(context_, lreStylesheet),
                  namespaceURI, 
                  localName, 
                  qName, 
                  atts);
  } // startLREAsStylesheet

  void startXSLTElement(const string_type& namespaceURI,
                        const string_type& localName,
                        const string_type& qName,
                        const AttributesT& atts)
  {
    if((localName == SC::import) || (localName == SC::include))
    {
      include_stylesheet(namespaceURI, localName, qName, atts);
      return;
    } // if ...
    
    for(const ChildElement<string_type, string_adaptor>* c = allowedChildren; 
        c->name != string_adaptor::empty_string(); 
        ++c)
      if(c->name == localName)
      {
        context_.push(0,
                      c->createHandler(context_),
                      namespaceURI, 
                      localName, 
                      qName, 
                      atts);
        return;
      } // if ...

    oops(qName);
  } // startXSLTElement

  void startForeignElement(const string_type& namespaceURI,
                           const string_type& localName,
                           const string_type& qName,
                           const AttributesT& atts)
  {
    context_.push(0,
                  new ForeignElementHandler<string_type, string_adaptor>(context_),
                  namespaceURI,
                  localName,
                  qName,
                  atts);
  } // startForeignElement

  void include_stylesheet(const string_type& namespaceURI,
                          const string_type& localName,
                          const string_type& qName,
                          const AttributesT& atts)
  {
    includer_.start_include(namespaceURI, localName, qName, atts);
  } // include_stylesheet

  void oops(const string_type& qName) const
  {
    throw SAX::SAXException("xsl:stylesheet does not allow " + string_adaptor::asStdString(qName) + " here.");  
  } // oops
  
  CompilationContextT& context_;
  DefaultHandlerT* child_;
  IncludeHandler<string_type, string_adaptor> includer_;
  bool top_;

  static const ChildElement<string_type, string_adaptor> allowedChildren[];
}; // class StylesheetHandler

template<class string_type, class string_adaptor>
const ChildElement<string_type, string_adaptor> StylesheetHandler<string_type, string_adaptor>::allowedChildren[] =
  {
    { SC::template_, CreateHandler<TemplateHandler<string_type, string_adaptor> > },
    { SC::param, CreateHandler<TopLevelVariableHandler<Param<string_type, string_adaptor> > >},
    { SC::variable, CreateHandler<TopLevelVariableHandler<Variable<string_type, string_adaptor> > > },
    { SC::output, CreateHandler<OutputHandler<string_type, string_adaptor> >},
    { SC::attribute_set, CreateHandler<NotImplementedYetHandler<string_type, string_adaptor> >},
    { SC::decimal_format, CreateHandler<NotImplementedYetHandler<string_type, string_adaptor> >},
    //"import"
    //"include"
    { SC::key, CreateHandler<KeyHandler<string_type, string_adaptor> >},
    { SC::namespace_alias, CreateHandler<NamespaceAliasHandler<string_type, string_adaptor> >},
    { SC::preserve_space, CreateHandler<NotImplementedYetHandler<string_type, string_adaptor> >},
    { SC::strip_space, CreateHandler<NotImplementedYetHandler<string_type, string_adaptor> >},
    { string_adaptor::empty_string(), 0 }
  }; // StylesheetHandler::allowedChildren

template<class string_type, class string_adaptor = Arabica::default_string_adaptor<string_type> >
class StylesheetCompiler 
{
public:
  typedef string_type string_typeT;
  typedef string_adaptor string_adaptorT;
  typedef SAX::InputSource<string_type, string_adaptor> InputSourceT;

  StylesheetCompiler() 
  {
  } // StylesheetCompiler

  ~StylesheetCompiler()
  {
  } // ~StylesheetCompiler

  std::auto_ptr<Stylesheet<string_type, string_adaptor> > compile(InputSourceT& source)
  {
    error_ = "";

    std::auto_ptr<CompiledStylesheet<string_type, string_adaptor> > stylesheet(new CompiledStylesheet<string_type, string_adaptor>());

    StylesheetParser<string_type, string_adaptor> parser;
    CompilationContext<string_type, string_adaptor> context(parser, *stylesheet.get());

    StylesheetHandler<string_type, string_adaptor> stylesheetHandler(context);
    parser.setContentHandler(stylesheetHandler);
    //parser.setErrorHandler(*this);
  
    //if(entityResolver_)
    //  parser.setEntityResolver(*entityResolver_);
    try {
      parser.parse(source);
    } // try
    catch(std::exception& ex)
    {
      error_ = ex.what();
      //std::cerr << "Compilation Failed : " << ex.what() << std::endl;
      stylesheet.reset();
    } // catch

    return std::auto_ptr<Stylesheet<string_type, string_adaptor> >(stylesheet.release());
  } // compile

  const std::string& error() const
  {
    return error_;
  } // error

private:
  std::string error_;
}; // class StylesheetCompiler

} // namespace XSLT
} // namespace Arabica

#endif // ARABICA_XSLT_STYLESHEETCOMPILER_HPP

