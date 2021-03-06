#ifndef test_nodeinsertbeforenewchildexists
#define test_nodeinsertbeforenewchildexists


/*
This C++ source file was generated for Arabica
from the source document contained in the W3C
DOM Conformance Test Suite.
The source document contained the following notice:


Copyright (c) 2001-2004 World Wide Web Consortium,
(Massachusetts Institute of Technology, Institut National de
Recherche en Informatique et en Automatique, Keio University). All
Rights Reserved. This program is distributed under the W3C's Software
Intellectual Property License. This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.
See W3C License http://www.w3.org/Consortium/Legal/ for more details.

*/
/**
 *     If the "newChild" is already in the tree, the
 *     "insertBefore(newChild,refChild)" method must first
 *     remove it before the insertion takes place.
 *     
 *     Insert a node Element ("employeeId") that is already
 *     present in the tree.   The existing node should be 
 *     removed first and the new one inserted.   The node is
 *     inserted at a different position in the tree to assure
 *     that it was indeed inserted.
* @author NIST
* @author Mary Brady
* @see <a href="http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core#ID-952280727">http://www.w3.org/TR/1998/REC-DOM-Level-1-19981001/level-one-core#ID-952280727</a>
*/

template<class string_type, class string_adaptor>
class nodeinsertbeforenewchildexists : public DOMTestCase<string_type, string_adaptor> 
{
    typedef DOMTestCase<string_type, string_adaptor> baseT;
   public: 
     nodeinsertbeforenewchildexists(std::string name) : baseT(name)
     {

       //   check if loaded documents are supported for content type
       const std::string contentType = baseT::getContentType();
    baseT::preload(contentType, "staff", true);
    }

  typedef typename Arabica::DOM::DOMImplementation<string_type, string_adaptor>  DOMImplementation;
  typedef typename Arabica::DOM::Document<string_type, string_adaptor> Document; 
  typedef typename Arabica::DOM::DocumentType<string_type, string_adaptor> DocumentType;
  typedef typename Arabica::DOM::DocumentFragment<string_type, string_adaptor> DocumentFragment; 
  typedef typename Arabica::DOM::Node<string_type, string_adaptor> Node;
  typedef typename Arabica::DOM::Element<string_type, string_adaptor> Element;
  typedef typename Arabica::DOM::Attr<string_type, string_adaptor> Attr;
  typedef typename Arabica::DOM::NodeList<string_type, string_adaptor> NodeList;
  typedef typename Arabica::DOM::NamedNodeMap<string_type, string_adaptor> NamedNodeMap;
  typedef typename Arabica::DOM::Entity<string_type, string_adaptor> Entity;
  typedef typename Arabica::DOM::EntityReference<string_type, string_adaptor> EntityReference;
  typedef typename Arabica::DOM::CharacterData<string_type, string_adaptor> CharacterData;
  typedef typename Arabica::DOM::CDATASection<string_type, string_adaptor> CDATASection;
  typedef typename Arabica::DOM::Text<string_type, string_adaptor> Text;
  typedef typename Arabica::DOM::Comment<string_type, string_adaptor> Comment;
  typedef typename Arabica::DOM::ProcessingInstruction<string_type, string_adaptor> ProcessingInstruction;
  typedef typename Arabica::DOM::Notation<string_type, string_adaptor> Notation;

  typedef typename Arabica::DOM::DOMException DOMException;
  typedef string_type String;
  typedef string_adaptor SA;
  typedef bool boolean;


   /*
    * Runs the test case.
    */
   void runTest()
   {
      Document doc;
      NodeList elementList;
      Node employeeNode;
      NodeList childList;
      Node refChild;
      Node newChild;
      Node child;
      int length;
      String childName;
      Node insertedNode;
      std::vector<string_type> expectedWhitespace;
      expectedWhitespace.push_back(SA::construct_from_utf8("#text"));
      expectedWhitespace.push_back(SA::construct_from_utf8("#text"));
      expectedWhitespace.push_back(SA::construct_from_utf8("name"));
      expectedWhitespace.push_back(SA::construct_from_utf8("#text"));
      expectedWhitespace.push_back(SA::construct_from_utf8("position"));
      expectedWhitespace.push_back(SA::construct_from_utf8("#text"));
      expectedWhitespace.push_back(SA::construct_from_utf8("salary"));
      expectedWhitespace.push_back(SA::construct_from_utf8("#text"));
      expectedWhitespace.push_back(SA::construct_from_utf8("gender"));
      expectedWhitespace.push_back(SA::construct_from_utf8("#text"));
      expectedWhitespace.push_back(SA::construct_from_utf8("employeeId"));
      expectedWhitespace.push_back(SA::construct_from_utf8("address"));
      expectedWhitespace.push_back(SA::construct_from_utf8("#text"));
      
      std::vector<string_type> expectedNoWhitespace;
      expectedNoWhitespace.push_back(SA::construct_from_utf8("name"));
      expectedNoWhitespace.push_back(SA::construct_from_utf8("position"));
      expectedNoWhitespace.push_back(SA::construct_from_utf8("salary"));
      expectedNoWhitespace.push_back(SA::construct_from_utf8("gender"));
      expectedNoWhitespace.push_back(SA::construct_from_utf8("employeeId"));
      expectedNoWhitespace.push_back(SA::construct_from_utf8("address"));
      
      std::vector<string_type> expected;
      
      std::vector<string_type> result;
      
      doc = (Document) baseT::load("staff", true);
      elementList = doc.getElementsByTagName(SA::construct_from_utf8("employee"));
      employeeNode = elementList.item(1);
      childList = employeeNode.getChildNodes();
      length = (int) childList.getLength();
      
      if (baseT::equals(6, length)) {
          expected =  expectedNoWhitespace;
      refChild = childList.item(5);
      newChild = childList.item(0);
      } else {
          expected =  expectedWhitespace;
      refChild = childList.item(11);
      newChild = childList.item(1);
      }
        
    insertedNode = employeeNode.insertBefore(newChild, refChild);
      for (unsigned int indexN65757 = 0; indexN65757 != childList.getLength(); indexN65757++) {
          child = (Node) childList.item(indexN65757);
    childName = child.getNodeName();
      result.push_back(childName);
        }
      baseT::assertEquals(expected, result, __LINE__, __FILE__);
  
   }
  
   /*
    *  Gets URI that identifies the test.
    */
   std::string getTargetURI() const
   {
      return "http://www.w3.org/2001/DOM-Test-Suite/level1/core/nodeinsertbeforenewchildexists";
   }
};

#endif
