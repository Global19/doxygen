/******************************************************************************
 *
 * $Id$
 *
 *
 * Copyright (C) 1997-2002 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#include "dochandler.h"
#include "debug.h"
#include <qmap.h>

//----------------------------------------------------------------------

class TypeNameMapper
{
  public:
    TypeNameMapper()
    {
      m_map.insert("see",       SimpleSectHandler::See);
      m_map.insert("return",    SimpleSectHandler::Return);
      m_map.insert("author",    SimpleSectHandler::Author);
      m_map.insert("version",   SimpleSectHandler::Version);
      m_map.insert("since",     SimpleSectHandler::Since);
      m_map.insert("date",      SimpleSectHandler::Date);
      m_map.insert("bug",       SimpleSectHandler::Bug);
      m_map.insert("note",      SimpleSectHandler::Note);
      m_map.insert("warning",   SimpleSectHandler::Warning);
      m_map.insert("par",       SimpleSectHandler::Par);
      m_map.insert("deprecated",SimpleSectHandler::Deprecated);
      m_map.insert("pre",       SimpleSectHandler::Pre);
      m_map.insert("post",      SimpleSectHandler::Post);
      m_map.insert("invariant", SimpleSectHandler::Invar);
      m_map.insert("remark",    SimpleSectHandler::Remark);
      m_map.insert("attention", SimpleSectHandler::Attention);
      m_map.insert("todo",      SimpleSectHandler::Todo);
      m_map.insert("test",      SimpleSectHandler::Test);
      m_map.insert("rcs",       SimpleSectHandler::RCS);
      m_map.insert("enumvalues",SimpleSectHandler::EnumValues);
      m_map.insert("examples",  SimpleSectHandler::Examples);
    }
    SimpleSectHandler::Types stringToType(const QString &typeStr)
    {
      return m_map[typeStr];
    }
  private:
    QMap<QString,SimpleSectHandler::Types> m_map;
};

class HighlightMapper
{
  public:
    HighlightMapper()
    {
      m_map.insert("comment",        HighlightHandler::Comment);
      m_map.insert("keyword",        HighlightHandler::Keyword);
      m_map.insert("keywordtype",    HighlightHandler::KeywordType);
      m_map.insert("keywordflow",    HighlightHandler::KeywordFlow);
      m_map.insert("charliteral",    HighlightHandler::CharLiteral);
      m_map.insert("stringliteral",  HighlightHandler::StringLiteral);
      m_map.insert("preprocessor",   HighlightHandler::Preprocessor);
    }
    HighlightHandler::HighlightKind stringToKind(const QString &kindStr)
    {
      return m_map[kindStr];
    }
  private:
    QMap<QString,HighlightHandler::HighlightKind> m_map;
};

static TypeNameMapper *s_typeMapper;
static HighlightMapper *s_highlightMapper;

void dochandler_init()
{
  s_typeMapper = new TypeNameMapper;
  s_highlightMapper = new HighlightMapper;
}

void dochandler_exit()
{
  delete s_typeMapper;
  delete s_highlightMapper;
}
  
//----------------------------------------------------------------------
// MarkupHandler
//----------------------------------------------------------------------

MarkupHandler::MarkupHandler(QList<DocImpl> &children,QString &curString)
  : m_children(children), m_curString(curString), m_curMarkup(IDocMarkup::Normal)
{
  addStartHandler("bold",this,&MarkupHandler::startBold);
  addEndHandler("bold",this,&MarkupHandler::endBold);
  
  addStartHandler("emphasis",this,&MarkupHandler::startEmphasis);
  addEndHandler("emphasis",this,&MarkupHandler::endEmphasis);
  
  addStartHandler("computeroutput",this,&MarkupHandler::startComputerOutput);
  addEndHandler("computeroutput",this,&MarkupHandler::endComputerOutput);

  addStartHandler("center",this,&MarkupHandler::startCenter);
  addEndHandler("center",this,&MarkupHandler::endCenter);

  addStartHandler("small",this,&MarkupHandler::startSmallFont);
  addEndHandler("small",this,&MarkupHandler::endSmallFont);

  addStartHandler("subscript",this,&MarkupHandler::startSubscript);
  addEndHandler("subscript",this,&MarkupHandler::endSubscript);

  addStartHandler("superscript",this,&MarkupHandler::startSuperscript);
  addEndHandler("superscript",this,&MarkupHandler::endSuperscript);
}

MarkupHandler::~MarkupHandler()
{
}

void MarkupHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,m_curMarkup));
    debug(2,"addTextNode() text=%s markup=%x\n",m_curString.data(),m_curMarkup);
    m_curString="";
  }
}

void MarkupHandler::startBold(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Bold,TRUE));
  m_curMarkup |= IDocMarkup::Bold;
}

void MarkupHandler::endBold()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Bold,FALSE));
  m_curMarkup &= ~IDocMarkup::Bold;
}

void MarkupHandler::startEmphasis(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Emphasis,TRUE));
  m_curMarkup |= IDocMarkup::Emphasis;
}

void MarkupHandler::endEmphasis()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Emphasis,FALSE));
  m_curMarkup &= ~IDocMarkup::Emphasis;
}

void MarkupHandler::startComputerOutput(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::ComputerOutput,TRUE));
  m_curMarkup |= IDocMarkup::ComputerOutput;
}

void MarkupHandler::endComputerOutput()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::ComputerOutput,FALSE));
  m_curMarkup &= ~IDocMarkup::ComputerOutput;
}

void MarkupHandler::startCenter(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Center,TRUE));
  m_curMarkup |= IDocMarkup::Center;
}

void MarkupHandler::endCenter()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Center,FALSE));
  m_curMarkup &= ~IDocMarkup::Center;
}

void MarkupHandler::startSmallFont(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::SmallFont,TRUE));
  m_curMarkup |= IDocMarkup::SmallFont;
}

void MarkupHandler::endSmallFont()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::SmallFont,FALSE));
  m_curMarkup &= ~IDocMarkup::SmallFont;
}

void MarkupHandler::startSubscript(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Subscript,TRUE));
  m_curMarkup |= IDocMarkup::Subscript;
}

void MarkupHandler::endSubscript()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Subscript,FALSE));
  m_curMarkup &= ~IDocMarkup::Subscript;
}

void MarkupHandler::startSuperscript(const QXmlAttributes & /*attrib*/)
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Superscript,TRUE));
  m_curMarkup |= IDocMarkup::Superscript;
}

void MarkupHandler::endSuperscript()
{
  addTextNode();
  m_children.append(new MarkupModifierNode(IDocMarkup::Superscript,FALSE));
  m_curMarkup &= ~IDocMarkup::Superscript;
}

//----------------------------------------------------------------------
// ListItemHandler
//----------------------------------------------------------------------

ListItemHandler::ListItemHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  
  addEndHandler("listitem",this,&ListItemHandler::endListItem);

  addStartHandler("para",this,&ListItemHandler::startParagraph);
}

ListItemHandler::~ListItemHandler()
{
}

void ListItemHandler::startListItem(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start list item handler\n");
}

void ListItemHandler::endListItem()
{
  debug(2,"end list item handler\n");
  m_parent->setDelegate(0);
}

void ListItemHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_children.append(parHandler);
}

IDocIterator *ListItemHandler::contents() const
{
  return new ListItemIterator(*this);
}

//----------------------------------------------------------------------
// OrderedListHandler
//----------------------------------------------------------------------

OrderedListHandler::OrderedListHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("orderedlist",this,&OrderedListHandler::endOrderedList);
  addStartHandler("listitem",this,&OrderedListHandler::startOrderedListItem);
}

OrderedListHandler::~OrderedListHandler()
{
}

void OrderedListHandler::startOrderedList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void OrderedListHandler::endOrderedList()
{
  m_parent->setDelegate(0);
}

void OrderedListHandler::startOrderedListItem(const QXmlAttributes& attrib)
{
  ListItemHandler *liHandler = new ListItemHandler(this);
  liHandler->startListItem(attrib);
  m_children.append(liHandler);
}

IDocIterator *OrderedListHandler::elements() const
{
  return new OrderedListIterator(*this);
}

//----------------------------------------------------------------------
// ItemizedListHandler
//----------------------------------------------------------------------

ItemizedListHandler::ItemizedListHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("itemizedlist",this,&ItemizedListHandler::endItemizedList);
  addStartHandler("listitem",this,&ItemizedListHandler::startItemizedListItem);
}

ItemizedListHandler::~ItemizedListHandler()
{
}

void ItemizedListHandler::startItemizedList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ItemizedListHandler::endItemizedList()
{
  m_parent->setDelegate(0);
}

void ItemizedListHandler::startItemizedListItem(const QXmlAttributes& attrib)
{
  ListItemHandler *liHandler = new ListItemHandler(this);
  liHandler->startListItem(attrib);
  m_children.append(liHandler);
}

IDocIterator *ItemizedListHandler::elements() const
{
  return new ItemizedListIterator(*this);
}

//----------------------------------------------------------------------
// ParameterHandler
//----------------------------------------------------------------------

ParameterHandler::ParameterHandler(IBaseHandler *parent) : 
  m_parent(parent), m_description(0)
{
  addEndHandler("parametername",this,&ParameterHandler::endParameterName);
  addEndHandler("parameterdescription",this,&ParameterHandler::endParameterDescription);
  addStartHandler("para",this,&ParameterHandler::startParagraph);
}

ParameterHandler::~ParameterHandler()
{
  delete m_description;
}

void ParameterHandler::startParameterName(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ParameterHandler::endParameterName()
{
  m_name = m_curString;
  debug(2,"parameter %s\n",m_name.data());
  m_curString="";
  m_parent->setDelegate(0);
}

void ParameterHandler::startParameterDescription(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
}

void ParameterHandler::endParameterDescription()
{
  m_parent->setDelegate(0);
}

void ParameterHandler::startParagraph(const QXmlAttributes& attrib)
{
  ASSERT(m_description==0);
  m_description = new ParagraphHandler(this);
  m_description->startParagraph(attrib);
}

//----------------------------------------------------------------------
// ParameterListHandler
//----------------------------------------------------------------------

ParameterListHandler::ParameterListHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  addEndHandler("parameterlist",this,&ParameterListHandler::endParameterList);
  addStartHandler("parametername",this,&ParameterListHandler::startParameterName);
  addStartHandler("parameterdescription",this,&ParameterListHandler::startParameterDescription);
  addStartHandler("title");
  addEndHandler("title");
  m_parameters.setAutoDelete(TRUE);
  m_curParam=0;
}

ParameterListHandler::~ParameterListHandler()
{
}

void ParameterListHandler::startParameterList(const QXmlAttributes& attrib)
{
  QString kind = attrib.value("kind");
  if (kind=="retval")         m_type=RetVal;
  else if (kind=="exception") m_type=Exception;
  else if (kind=="param")     m_type=Param;
  else
  {
    debug(1,"Error: invalid parameterlist type: %s\n",kind.data());
  }
  debug(2,"parameterlist kind=%s\n",kind.data());
  m_parent->setDelegate(this);
}

void ParameterListHandler::endParameterList()
{
  m_parent->setDelegate(0);
}

void ParameterListHandler::startParameterName(const QXmlAttributes& attrib)
{
  m_curParam = new ParameterHandler(this);
  m_parameters.append(m_curParam);
  m_curParam->startParameterName(attrib);
}

void ParameterListHandler::startParameterDescription(const QXmlAttributes& attrib)
{
  ASSERT(m_curParam!=0);
  m_curParam->startParameterDescription(attrib);
}

IDocIterator *ParameterListHandler::params() const
{
  return new ParameterListIterator(*this);
}

//----------------------------------------------------------------------
// LinkHandler
//----------------------------------------------------------------------

LinkHandler::LinkHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("link",this,&LinkHandler::endLink);
}

LinkHandler::~LinkHandler()
{
}

void LinkHandler::startLink(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"Start link\n");
  m_ref = attrib.value("linkend");
  m_curString="";
}

void LinkHandler::endLink()
{
  m_text = m_curString;
  m_curString="";
  m_parent->setDelegate(0);
  debug(2,"End link\n");
}

//----------------------------------------------------------------------
// EMailHandler
//----------------------------------------------------------------------

EMailHandler::EMailHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("email",this,&EMailHandler::endEMail);
}

EMailHandler::~EMailHandler()
{
}

void EMailHandler::startEMail(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start email\n");
  m_curString="";
}

void EMailHandler::endEMail()
{
  m_address = m_curString;
  m_curString="";
  m_parent->setDelegate(0);
  debug(2,"End email\n");
}

//----------------------------------------------------------------------
// ULinkHandler
//----------------------------------------------------------------------

ULinkHandler::ULinkHandler(IBaseHandler *parent)
  :  m_parent(parent)
{
  addEndHandler("ulink",this,&ULinkHandler::endULink);
}

ULinkHandler::~ULinkHandler()
{
}

void ULinkHandler::startULink(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"Start ulink\n");
  m_url = attrib.value("url");
  m_curString="";
}

void ULinkHandler::endULink()
{
  m_text = m_curString;
  m_curString="";
  m_parent->setDelegate(0);
  debug(2,"End ulink\n");
}

//----------------------------------------------------------------------
// LineBreakHandler
//----------------------------------------------------------------------

LineBreakHandler::LineBreakHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("linebreak",this,&LineBreakHandler::endLineBreak);
}

LineBreakHandler::~LineBreakHandler()
{
}

void LineBreakHandler::startLineBreak(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start linebreak\n");
}

void LineBreakHandler::endLineBreak()
{
  m_parent->setDelegate(0);
  debug(2,"End linebreak\n");
}

//----------------------------------------------------------------------
// HRulerHandler
//----------------------------------------------------------------------

HRulerHandler::HRulerHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("hruler",this,&HRulerHandler::endHRuler);
}

HRulerHandler::~HRulerHandler()
{
}

void HRulerHandler::startHRuler(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start hruler\n");
}

void HRulerHandler::endHRuler()
{
  m_parent->setDelegate(0);
  debug(2,"End hruler\n");
}

//----------------------------------------------------------------------
// RefHandler
//----------------------------------------------------------------------

RefHandler::RefHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("ref",this,&RefHandler::endRef);
}

RefHandler::~RefHandler()
{
}

void RefHandler::startRef(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_refId = attrib.value("refid");
  m_extId = attrib.value("external");
  ASSERT(attrib.value("kindref")=="compound" || 
         attrib.value("kindref")=="member");
  m_targetKind = attrib.value("kindref")=="compound" ?  Compound : Member;
  debug(2,"Start ref refId=%s\n",m_refId.data());
  m_curString="";
}

void RefHandler::endRef()
{
  m_linkText = m_curString;
  m_parent->setDelegate(0);
  debug(2,"End ref: text=`%s'\n",m_linkText.data());
}


//----------------------------------------------------------------------
// TitleHandler
//----------------------------------------------------------------------

TitleHandler::TitleHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  m_markupHandler = new MarkupHandler(m_children,m_curString);
  setFallBackHandler(m_markupHandler);
  addStartHandler("ref",this,&TitleHandler::startRef);
  addEndHandler("title",this,&TitleHandler::endTitle);
}

TitleHandler::~TitleHandler()
{
  delete m_markupHandler;
}

void TitleHandler::startTitle(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"Start title\n");
  m_curString="";
}

void TitleHandler::endTitle()
{
  addTextNode();
  m_parent->setDelegate(0);
  debug(2,"End title\n");
}

void TitleHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,m_markupHandler->markup()));
    debug(2,"addTextNode() text=\"%s\" markup=%x\n",
        m_curString.data(),m_markupHandler->markup());
    m_curString="";
  }
}

void TitleHandler::startRef(const QXmlAttributes& attrib)
{
  RefHandler *ref = new RefHandler(this);
  ref->startRef(attrib);
  m_children.append(ref);
}

IDocIterator *TitleHandler::title() const
{
  return new TitleIterator(*this);
}

//----------------------------------------------------------------------
// SimpleSectHandler
//----------------------------------------------------------------------

SimpleSectHandler::SimpleSectHandler(IBaseHandler *parent)
  : m_parent(parent), m_paragraph(0), m_title(0)
{
  addStartHandler("title",this,&SimpleSectHandler::startTitle);
  addStartHandler("para",this,&SimpleSectHandler::startParagraph);
  addEndHandler("simplesect",this,&SimpleSectHandler::endSimpleSect);
}

SimpleSectHandler::~SimpleSectHandler()
{
}

void SimpleSectHandler::startSimpleSect(const QXmlAttributes& attrib)
{
  m_typeString = attrib.value("kind");
  m_type = s_typeMapper->stringToType(m_typeString);
  debug(2,"start simple section %s\n",m_typeString.data());
  m_parent->setDelegate(this);
}

void SimpleSectHandler::endSimpleSect()
{
  debug(2,"end simple section\n");
  m_parent->setDelegate(0);
}

void SimpleSectHandler::startTitle(const QXmlAttributes& attrib)
{
  ASSERT(m_title==0);
  m_title = new TitleHandler(this);
  m_title->startTitle(attrib);
}

void SimpleSectHandler::startParagraph(const QXmlAttributes& attrib)
{
  ASSERT(m_paragraph==0);
  m_paragraph = new ParagraphHandler(this);
  m_paragraph->startParagraph(attrib);
}

//----------------------------------------------------------------------
// VariableListEntryHandler
//----------------------------------------------------------------------

VariableListEntryHandler::VariableListEntryHandler(IBaseHandler *parent)
   : m_parent(parent), m_description(0)
{
  addStartHandler("term",this,&VariableListEntryHandler::startTerm);
  addEndHandler("term",this,&VariableListEntryHandler::endTerm);
  addStartHandler("para",this,&VariableListEntryHandler::startParagraph);
  addEndHandler("varlistentry",this,&VariableListEntryHandler::endVarListEntry);
  addEndHandler("listitem",this,&VariableListEntryHandler::endListItem);
}

VariableListEntryHandler::~VariableListEntryHandler()
{
  delete m_description;
}

void VariableListEntryHandler::startVarListEntry(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start varlistentry\n");
}

void VariableListEntryHandler::endVarListEntry()
{
  m_parent->setDelegate(0);
  debug(2,"end varlistentry\n");
}

void VariableListEntryHandler::startListItem(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start listitem\n");
}

void VariableListEntryHandler::endListItem()
{
  m_parent->setDelegate(0);
  debug(2,"end listitem\n");
}

void VariableListEntryHandler::startTerm(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void VariableListEntryHandler::endTerm()
{
  m_term = m_curString;
  debug(2,"term=%s\n",m_term.data());
}

void VariableListEntryHandler::startParagraph(const QXmlAttributes& attrib)
{
  ASSERT(m_description==0);
  m_description = new ParagraphHandler(this);
  m_description->startParagraph(attrib);
}



//----------------------------------------------------------------------
// VariableListHandler
//----------------------------------------------------------------------

VariableListHandler::VariableListHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  m_entries.setAutoDelete(TRUE);
  addStartHandler("varlistentry",this,&VariableListHandler::startVarListEntry);
  addStartHandler("listitem",this,&VariableListHandler::startListItem);
  addEndHandler("variablelist",this,&VariableListHandler::endVariableList);
}

VariableListHandler::~VariableListHandler()
{
}

void VariableListHandler::startVariableList(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start variablelist\n");
}

void VariableListHandler::endVariableList()
{
  debug(2,"end variablelist\n");
  m_parent->setDelegate(0);
}

void VariableListHandler::startVarListEntry(const QXmlAttributes& attrib)
{
  VariableListEntryHandler *vle = new VariableListEntryHandler(this);
  vle->startVarListEntry(attrib);
  m_curEntry = vle;
  m_entries.append(vle);
}

void VariableListHandler::startListItem(const QXmlAttributes& attrib)
{
  ASSERT(m_curEntry!=0);
  m_curEntry->startListItem(attrib);
}

IDocIterator *VariableListHandler::entries() const
{
  return new VariableListIterator(*this);
}

//----------------------------------------------------------------------
// HighlightHandler
//----------------------------------------------------------------------

HighlightHandler::HighlightHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("highlight",this,&HighlightHandler::endHighlight);
  addStartHandler("ref",this,&HighlightHandler::startRef);
  m_hl = IDocHighlight::Invalid;
}

HighlightHandler::~HighlightHandler()
{
}

void HighlightHandler::startHighlight(const QXmlAttributes& attrib)
{
  m_hlString = attrib.value("class");
  m_hl = s_highlightMapper->stringToKind(m_hlString);
  m_curString="";
  m_parent->setDelegate(this);
}

void HighlightHandler::endHighlight()
{
  addTextNode();
  debug(2,"highlight class=`%s'\n",m_hlString.data());
  m_parent->setDelegate(0);
}

void HighlightHandler::startRef(const QXmlAttributes& attrib)
{
  addTextNode();
  RefHandler *rh = new RefHandler(this);
  m_children.append(rh);
  rh->startRef(attrib);
}

void HighlightHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,IDocMarkup::Normal));
    debug(2,"addTextNode() text=\"%s\"\n",
        m_curString.data());
    m_curString="";
  }
}

IDocIterator *HighlightHandler::codeElements() const
{
  return new HighlightIterator(*this);
}

//----------------------------------------------------------------------
// CodeLineHandler
//----------------------------------------------------------------------

CodeLineHandler::CodeLineHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("codeline",this,&CodeLineHandler::endCodeLine);
  addEndHandler("linenumber",this,&CodeLineHandler::endLineNumber);
  addStartHandler("highlight",this,&CodeLineHandler::startHighlight);
  addStartHandler("ref",this,&CodeLineHandler::startRef);
  m_lineNumber = 0;
}

CodeLineHandler::~CodeLineHandler()
{
}

void CodeLineHandler::startCodeLine(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start codeline\n");
}

void CodeLineHandler::endCodeLine()
{
  addTextNode();
  debug(2,"end codeline\n");
  m_parent->setDelegate(0);
}

void CodeLineHandler::startLineNumber(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"start linenumber\n");
  m_lineNumber = attrib.value("line").toInt();
  m_refId = attrib.value("refid");
}

void CodeLineHandler::endLineNumber()
{
  m_parent->setDelegate(0);
}

void CodeLineHandler::startHighlight(const QXmlAttributes& attrib)
{
  addTextNode();
  HighlightHandler *hlh = new HighlightHandler(this);
  m_children.append(hlh);
  hlh->startHighlight(attrib);
}

void CodeLineHandler::startRef(const QXmlAttributes& attrib)
{
  addTextNode();
  RefHandler *rh = new RefHandler(this);
  m_children.append(rh);
  rh->startRef(attrib);
}

void CodeLineHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,IDocMarkup::Normal));
    debug(2,"addTextNode() text=\"%s\"\n",
        m_curString.data());
    m_curString="";
  }
}

IDocIterator *CodeLineHandler::codeElements() const
{
  return new CodeLineIterator(*this);
}


//----------------------------------------------------------------------
// ProgramListingHandler
//----------------------------------------------------------------------

ProgramListingHandler::ProgramListingHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  m_hasLineNumber=FALSE;
  addEndHandler("programlisting",this,&ProgramListingHandler::endProgramListing);

  addStartHandler("linenumber",this,&ProgramListingHandler::startLineNumber);
  addStartHandler("codeline",this,&ProgramListingHandler::startCodeLine);
}

ProgramListingHandler::~ProgramListingHandler()
{
}

void ProgramListingHandler::startProgramListing(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start programlisting\n");
}

void ProgramListingHandler::endProgramListing()
{
  debug(2,"end programlisting\n");
  m_parent->setDelegate(0);
}

void ProgramListingHandler::startLineNumber(const QXmlAttributes& attrib)
{
  CodeLineHandler *clh = new CodeLineHandler(this);
  m_children.append(clh);
  m_hasLineNumber=TRUE;
  clh->startLineNumber(attrib);
}
  
void ProgramListingHandler::startCodeLine(const QXmlAttributes& attrib)
{
  CodeLineHandler *clh = 0;
  if (!m_hasLineNumber) 
  {
    clh = new CodeLineHandler(this);
    m_children.append(clh);
  }
  else
  {
    clh = m_children.getLast();
  }
  ASSERT(clh!=0);
  clh->startCodeLine(attrib);
  m_hasLineNumber=FALSE; 
}

IDocIterator *ProgramListingHandler::codeLines() const
{
  return new ProgramListingIterator(*this);
}



//----------------------------------------------------------------------
// FormulaHandler
//----------------------------------------------------------------------

FormulaHandler::FormulaHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("formula",this,&FormulaHandler::endFormula);
}

FormulaHandler::~FormulaHandler()
{
}

void FormulaHandler::startFormula(const QXmlAttributes& attrib)
{
  m_id = attrib.value("id");
  m_curString="";
  m_parent->setDelegate(this);
}

void FormulaHandler::endFormula()
{
  m_text = m_curString;
  debug(2,"formula id=`%s' text=`%s'\n",m_id.data(),m_text.data());
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// ImageHandler
//----------------------------------------------------------------------

ImageHandler::ImageHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("image",this,&ImageHandler::endImage);
}

ImageHandler::~ImageHandler()
{
}

void ImageHandler::startImage(const QXmlAttributes& attrib)
{
  m_name = attrib.value("name");
  m_curString="";
  m_parent->setDelegate(this);
}

void ImageHandler::endImage()
{
  m_caption = m_curString;
  debug(2,"image name=`%s' caption=`%s'\n",m_name.data(),m_caption.data());
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// DotFileHandler
//----------------------------------------------------------------------

DotFileHandler::DotFileHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("image",this,&DotFileHandler::endDotFile);
}

DotFileHandler::~DotFileHandler()
{
}

void DotFileHandler::startDotFile(const QXmlAttributes& attrib)
{
  m_name = attrib.value("name");
  m_curString="";
  m_parent->setDelegate(this);
}

void DotFileHandler::endDotFile()
{
  m_caption = m_curString;
  debug(2,"image name=`%s' caption=`%s'\n",m_name.data(),m_caption.data());
  m_parent->setDelegate(0);
}

//----------------------------------------------------------------------
// IndexEntryHandler
//----------------------------------------------------------------------

IndexEntryHandler::IndexEntryHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  addEndHandler("indexentry",this,&IndexEntryHandler::endIndexEntry);
  addStartHandler("primaryie",this,&IndexEntryHandler::startPrimaryIE);
  addEndHandler("primaryie",this,&IndexEntryHandler::endPrimaryIE);
  addStartHandler("secondaryie",this,&IndexEntryHandler::startSecondaryIE);
  addEndHandler("secondaryie",this,&IndexEntryHandler::endSecondaryIE);
}

IndexEntryHandler::~IndexEntryHandler()
{
}

void IndexEntryHandler::startIndexEntry(const QXmlAttributes& /*attrib*/)
{
  debug(2,"start index entry\n");
  m_parent->setDelegate(this);
}

void IndexEntryHandler::endIndexEntry()
{
  debug(2,"index entry primary=`%s' secondary=`%s'\n",
      m_primary.data(),m_secondary.data());
  m_parent->setDelegate(0);
}

void IndexEntryHandler::startPrimaryIE(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void IndexEntryHandler::endPrimaryIE()
{
  m_primary = m_curString;
}

void IndexEntryHandler::startSecondaryIE(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void IndexEntryHandler::endSecondaryIE()
{
  m_secondary = m_curString;
}

//----------------------------------------------------------------------
// EntryHandler
//----------------------------------------------------------------------

EntryHandler::EntryHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("entry",this,&EntryHandler::endEntry);
  addStartHandler("para",this,&EntryHandler::startParagraph);
}

EntryHandler::~EntryHandler()
{
}

void EntryHandler::startEntry(const QXmlAttributes&)
{
  m_parent->setDelegate(this);
}

void EntryHandler::endEntry()
{
  m_parent->setDelegate(0);
}

void EntryHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *ph = new ParagraphHandler(this);
  ph->startParagraph(attrib);
  m_children.append(ph);
}

IDocIterator *EntryHandler::contents() const
{
  return new EntryIterator(*this);
}

//----------------------------------------------------------------------
// RowHandler
//----------------------------------------------------------------------

RowHandler::RowHandler(IBaseHandler *parent)
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("row",this,&RowHandler::endRow);
  addStartHandler("entry",this,&RowHandler::startEntry);
}

RowHandler::~RowHandler()
{
}

void RowHandler::startRow(const QXmlAttributes&)
{
  m_parent->setDelegate(this);
}

void RowHandler::endRow()
{
  m_parent->setDelegate(0);
}

void RowHandler::startEntry(const QXmlAttributes& attrib)
{
  EntryHandler *eh = new EntryHandler(this);
  eh->startEntry(attrib);
  m_children.append(eh);
}

IDocIterator *RowHandler::entries() const
{
  return new RowIterator(*this);
}

//----------------------------------------------------------------------
// TableHandler
//----------------------------------------------------------------------

TableHandler::TableHandler(IBaseHandler *parent)
  :  m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("table",this,&TableHandler::endTable);
  addStartHandler("row",this,&TableHandler::startRow);
  addStartHandler("caption",this,&TableHandler::startCaption);
  addEndHandler("caption",this,&TableHandler::endCaption);
}

TableHandler::~TableHandler()
{
}

void TableHandler::startTable(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  m_numColumns = attrib.value("cols").toInt();
  debug(2,"table cols=%d\n",m_numColumns);
}

void TableHandler::endTable()
{
  m_parent->setDelegate(0);
}

void TableHandler::startRow(const QXmlAttributes& attrib)
{
  RowHandler *rh = new RowHandler(this);
  rh->startRow(attrib);
  m_children.append(rh);
}

void TableHandler::startCaption(const QXmlAttributes& /*attrib*/)
{
  m_curString="";
}

void TableHandler::endCaption()
{
  m_caption = m_curString;
}

IDocIterator *TableHandler::rows() const
{
  return new TableIterator(*this);
}

//----------------------------------------------------------------------
// PreformattedHandler
//----------------------------------------------------------------------

PreformattedHandler::PreformattedHandler(IBaseHandler *parent)
  :  m_parent(parent)
{
  m_children.setAutoDelete(TRUE);
  addEndHandler("preformatted",this,&PreformattedHandler::endPreformatted);
}

PreformattedHandler::~PreformattedHandler()
{
}

void PreformattedHandler::startPreformatted(const QXmlAttributes&)
{
  m_parent->setDelegate(this);
}

void PreformattedHandler::endPreformatted()
{
  m_parent->setDelegate(0);
}

IDocIterator *PreformattedHandler::contents() const
{
  return new PreformattedIterator(*this);
}

//----------------------------------------------------------------------
// SymbolHandler
//----------------------------------------------------------------------

SymbolHandler::SymbolHandler(IBaseHandler *parent,Types type)
  :  m_parent(parent), m_letter('\0'), m_type(type)
{
  addEndHandler("symbol");
  switch (type)
  {
    case IDocSymbol::Invalid: m_typeString="invalid"; break;
    case IDocSymbol::Umlaut:  m_typeString="umlaut"; break;
    case IDocSymbol::Acute:   m_typeString="acute"; break;
    case IDocSymbol::Grave:   m_typeString="grave"; break;
    case IDocSymbol::Circ:    m_typeString="circ"; break;
    case IDocSymbol::Tilde:   m_typeString="tilde"; break;
    case IDocSymbol::Szlig:   m_typeString="szlig"; break;
    case IDocSymbol::Cedil:   m_typeString="cedil"; break;
    case IDocSymbol::Ring:    m_typeString="ring"; break;
    case IDocSymbol::Nbsp:    m_typeString="nbsp"; break;
    case IDocSymbol::Copy:    m_typeString="copy"; break;
  }
}

SymbolHandler::~SymbolHandler()
{
}

void SymbolHandler::startSymbol(const QXmlAttributes& attrib)
{
  QString ls = attrib.value("char");
  if (!ls.isEmpty()) m_letter = ls.latin1()[0];
}

//----------------------------------------------------------------------
// ParagraphHandler
//----------------------------------------------------------------------

ParagraphHandler::ParagraphHandler(IBaseHandler *parent) 
  : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);

  m_markupHandler = new MarkupHandler(m_children,m_curString);
  setFallBackHandler(m_markupHandler);

  addEndHandler("para",this,&ParagraphHandler::endParagraph);

  addStartHandler("itemizedlist",this,&ParagraphHandler::startItemizedList);
  addStartHandler("orderedlist",this,&ParagraphHandler::startOrderedList);
  addStartHandler("parameterlist",this,&ParagraphHandler::startParameterList);
  addStartHandler("simplesect",this,&ParagraphHandler::startSimpleSect);
  addStartHandler("ref",this,&ParagraphHandler::startRef);
  addStartHandler("variablelist",this,&ParagraphHandler::startVariableList);
  addStartHandler("hruler",this,&ParagraphHandler::startHRuler);
  addStartHandler("linebreak",this,&ParagraphHandler::startLineBreak);
  addStartHandler("ulink",this,&ParagraphHandler::startULink);
  addStartHandler("email",this,&ParagraphHandler::startEMail);
  addStartHandler("link",this,&ParagraphHandler::startLink);
  addStartHandler("programlisting",this,&ParagraphHandler::startProgramListing);
  addStartHandler("formula",this,&ParagraphHandler::startFormula);
  addStartHandler("image",this,&ParagraphHandler::startImage);
  addStartHandler("dotfile",this,&ParagraphHandler::startDotFile);
  addStartHandler("indexentry",this,&ParagraphHandler::startIndexEntry);
  addStartHandler("table",this,&ParagraphHandler::startTable);
  addStartHandler("preformatted",this,&ParagraphHandler::startPreformatted);
  addStartHandler("umlaut",this,&ParagraphHandler::startUmlaut);
  addStartHandler("acute",this,&ParagraphHandler::startAcute);
  addStartHandler("grave",this,&ParagraphHandler::startGrave);
  addStartHandler("circ",this,&ParagraphHandler::startCirc);
  addStartHandler("tilde",this,&ParagraphHandler::startTilde);
  addStartHandler("szlig",this,&ParagraphHandler::startSzlig);
  addStartHandler("cedil",this,&ParagraphHandler::startCedil);
  addStartHandler("ring",this,&ParagraphHandler::startRing);
  addStartHandler("nbsp",this,&ParagraphHandler::startNbsp);
  addStartHandler("copy",this,&ParagraphHandler::startCopy);
}

ParagraphHandler::~ParagraphHandler()
{
  delete m_markupHandler;
}

void ParagraphHandler::startParagraph(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"para\n");
}

void ParagraphHandler::endParagraph()
{
  addTextNode();
  debug(2,"end para\n");
  m_parent->setDelegate(0);
}

void ParagraphHandler::startItemizedList(const QXmlAttributes& attrib)
{
  addTextNode();
  ItemizedListHandler *listHandler = new ItemizedListHandler(this);
  listHandler->startItemizedList(attrib);
  m_children.append(listHandler);
}

void ParagraphHandler::startOrderedList(const QXmlAttributes& attrib)
{
  addTextNode();
  OrderedListHandler *listHandler = new OrderedListHandler(this);
  listHandler->startOrderedList(attrib);
  m_children.append(listHandler);
}

void ParagraphHandler::startParameterList(const QXmlAttributes& attrib)
{
  addTextNode();
  ParameterListHandler *parListHandler = new ParameterListHandler(this);
  parListHandler->startParameterList(attrib);
  m_children.append(parListHandler);
}

void ParagraphHandler::startSimpleSect(const QXmlAttributes& attrib)
{
  addTextNode();
  SimpleSectHandler *sectHandler = new SimpleSectHandler(this);
  sectHandler->startSimpleSect(attrib);
  m_children.append(sectHandler);
}

void ParagraphHandler::startRef(const QXmlAttributes& attrib)
{
  addTextNode();
  RefHandler *ref = new RefHandler(this);
  ref->startRef(attrib);
  m_children.append(ref);
}

void ParagraphHandler::startVariableList(const QXmlAttributes& attrib)
{
  addTextNode();
  VariableListHandler *vl = new VariableListHandler(this);
  vl->startVariableList(attrib);
  m_children.append(vl);
}

void ParagraphHandler::startHRuler(const QXmlAttributes& attrib)
{
  addTextNode();
  HRulerHandler *hr = new HRulerHandler(this);
  hr->startHRuler(attrib);
  m_children.append(hr);
}

void ParagraphHandler::startLineBreak(const QXmlAttributes& attrib)
{
  addTextNode();
  LineBreakHandler *lb = new LineBreakHandler(this);
  lb->startLineBreak(attrib);
  m_children.append(lb);
}

void ParagraphHandler::startULink(const QXmlAttributes& attrib)
{
  addTextNode();
  ULinkHandler *uh = new ULinkHandler(this);
  uh->startULink(attrib);
  m_children.append(uh);
}

void ParagraphHandler::startEMail(const QXmlAttributes& attrib)
{
  addTextNode();
  EMailHandler *eh = new EMailHandler(this);
  eh->startEMail(attrib);
  m_children.append(eh);
}

void ParagraphHandler::startLink(const QXmlAttributes& attrib)
{
  addTextNode();
  LinkHandler *lh = new LinkHandler(this);
  lh->startLink(attrib);
  m_children.append(lh);
}

void ParagraphHandler::startProgramListing(const QXmlAttributes& attrib)
{
  addTextNode();
  ProgramListingHandler *pl = new ProgramListingHandler(this);
  pl->startProgramListing(attrib);
  m_children.append(pl);
}

void ParagraphHandler::startFormula(const QXmlAttributes& attrib)
{
  addTextNode();
  FormulaHandler *fh = new FormulaHandler(this);
  fh->startFormula(attrib);
  m_children.append(fh);
}

void ParagraphHandler::startImage(const QXmlAttributes& attrib)
{
  addTextNode();
  ImageHandler *ih = new ImageHandler(this);
  ih->startImage(attrib);
  m_children.append(ih);
}

void ParagraphHandler::startDotFile(const QXmlAttributes& attrib)
{
  addTextNode();
  DotFileHandler *df = new DotFileHandler(this);
  df->startDotFile(attrib);
  m_children.append(df);
}

void ParagraphHandler::startIndexEntry(const QXmlAttributes& attrib)
{
  addTextNode();
  IndexEntryHandler *df = new IndexEntryHandler(this);
  df->startIndexEntry(attrib);
  m_children.append(df);
}

void ParagraphHandler::startTable(const QXmlAttributes& attrib)
{
  addTextNode();
  TableHandler *th = new TableHandler(this);
  th->startTable(attrib);
  m_children.append(th);
}

void ParagraphHandler::startPreformatted(const QXmlAttributes& attrib)
{
  addTextNode();
  PreformattedHandler *ph = new PreformattedHandler(this);
  ph->startPreformatted(attrib);
  m_children.append(ph);
}

void ParagraphHandler::startUmlaut(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Umlaut);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startAcute(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Acute);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startGrave(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Grave);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startCirc(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Circ);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startTilde(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Tilde);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startSzlig(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Szlig);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startCedil(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Cedil);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startRing(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Ring);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startNbsp(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Nbsp);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::startCopy(const QXmlAttributes& attrib)
{
  addTextNode();
  SymbolHandler *sh = new SymbolHandler(this,IDocSymbol::Copy);
  sh->startSymbol(attrib);
  m_children.append(sh);
}

void ParagraphHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,m_markupHandler->markup()));
    debug(2,"addTextNode() text=\"%s\" markup=%x\n",
        m_curString.data(),m_markupHandler->markup());
    m_curString="";
  }
}

IDocIterator *ParagraphHandler::contents() const
{
  return new ParagraphIterator(*this);
}

//----------------------------------------------------------------------
// DocSectionHandler
//----------------------------------------------------------------------

DocSectionHandler::DocSectionHandler(IBaseHandler *parent,int level)
  : m_parent(parent), m_level(level)
{
  m_children.setAutoDelete(TRUE);
  m_markupHandler = new MarkupHandler(m_children,m_curString);
  setFallBackHandler(m_markupHandler);
  addStartHandler("ref",this,&DocSectionHandler::startRef);
  QString sectionKey;
  sectionKey.sprintf("sect%d",level);
  addEndHandler(sectionKey,this,&DocSectionHandler::endDocSection);
}

DocSectionHandler::~DocSectionHandler()
{
  delete m_markupHandler;
}

void DocSectionHandler::startDocSection(const QXmlAttributes& attrib)
{
  m_parent->setDelegate(this);
  debug(2,"Start docsection\n");
  m_id = attrib.value("id");
  m_curString="";
}

void DocSectionHandler::endDocSection()
{
  addTextNode();
  m_parent->setDelegate(0);
  debug(2,"End docsection\n");
}

void DocSectionHandler::addTextNode()
{
  if (!m_curString.isEmpty())
  {
    m_children.append(new TextNode(m_curString,m_markupHandler->markup()));
    debug(2,"addTextNode() text=\"%s\" markup=%x\n",
        m_curString.data(),m_markupHandler->markup());
    m_curString="";
  }
}

void DocSectionHandler::startRef(const QXmlAttributes& attrib)
{
  RefHandler *ref = new RefHandler(this);
  ref->startRef(attrib);
  m_children.append(ref);
}

IDocIterator *DocSectionHandler::title() const
{
  return new DocSectionIterator(*this);
}

//----------------------------------------------------------------------
// DocHandler
//----------------------------------------------------------------------

DocHandler::DocHandler(IBaseHandler *parent) : m_parent(parent)
{
  m_children.setAutoDelete(TRUE);

  addEndHandler("briefdescription",this,&DocHandler::endDoc);
  addEndHandler("detaileddescription",this,&DocHandler::endDoc);
  addEndHandler("internal");

  addStartHandler("para",this,&DocHandler::startParagraph);
  addStartHandler("sect1",this,&DocHandler::startSect1);
  addStartHandler("sect2",this,&DocHandler::startSect2);
  addStartHandler("sect3",this,&DocHandler::startSect3);
  addStartHandler("sect4",this,&DocHandler::startSect4);
  addStartHandler("sect5",this,&DocHandler::startSect5);
  addStartHandler("sect6",this,&DocHandler::startSect6);
  addStartHandler("title",this,&DocHandler::startTitle);
  addStartHandler("internal");
}

DocHandler::~DocHandler()
{
}

void DocHandler::startDoc(const QXmlAttributes& /*attrib*/)
{
  m_parent->setDelegate(this);
  debug(2,"start dochandler\n");
}

void DocHandler::endDoc()
{
  debug(2,"end dochandler\n");
  m_parent->setDelegate(0);
}

void DocHandler::startParagraph(const QXmlAttributes& attrib)
{
  ParagraphHandler *parHandler = new ParagraphHandler(this);
  parHandler->startParagraph(attrib);
  m_children.append(parHandler);
}

void DocHandler::startSect1(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,1);
  secHandler->startDocSection(attrib);
  m_children.append(secHandler);
}

void DocHandler::startSect2(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,2);
  secHandler->startDocSection(attrib);
  m_children.append(secHandler);
}

void DocHandler::startSect3(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,3);
  secHandler->startDocSection(attrib);
  m_children.append(secHandler);
}

void DocHandler::startSect4(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,4);
  secHandler->startDocSection(attrib);
  m_children.append(secHandler);
}

void DocHandler::startSect5(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,5);
  secHandler->startDocSection(attrib);
  m_children.append(secHandler);
}

void DocHandler::startSect6(const QXmlAttributes& attrib)
{
  DocSectionHandler *secHandler = new DocSectionHandler(this,6);
  secHandler->startDocSection(attrib);
  m_children.append(secHandler);
}

void DocHandler::startTitle(const QXmlAttributes& attrib)
{
  TitleHandler *titleHandler = new TitleHandler(this);
  titleHandler->startTitle(attrib);
  m_children.append(titleHandler);
}

IDocIterator *DocHandler::contents() const
{
  return new DocIterator(*this);
}

