/********************************************************************************************
*
* FoldingText.java
* 
* Description:	
* 
* Created on 	May 1, 2005
* @author 		Douglas Pearson
* 
* Developed by ThreePenny Software <a href="http://www.threepenny.net">www.threepenny.net</a>
********************************************************************************************/
package helpers;

import java.util.ArrayList;

import org.eclipse.swt.widgets.*;
import org.eclipse.swt.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.layout.*;

/************************************************************************
 * 
 * A widget that consists of a scrolling text window and a small bar with
 * icons for 'folding' the text (i.e. expanding and contracting) sections
 * of the text.
 * 
 * This is very similar to a view offered by Eclipse, but only depends on SWT
 * and may be higher performance if we get it right.
 * 
 ************************************************************************/
public class FoldingText
{
	protected Text		m_Text ;
	protected Canvas 	m_IconBar ;
	protected Composite m_Container ;
	protected FoldingTextDoc m_FoldingDoc = new FoldingTextDoc(this) ;

	public static class FoldingTextDoc
	{
		protected	ArrayList	m_TextBlocks = new ArrayList() ;
		protected	int			m_ShowFilter ;
		protected	int			m_HideFilter ;
		
		protected 	FoldingText	m_FoldingText ;
		protected	Block		m_LastBlock ;
		
		public FoldingTextDoc(FoldingText text)
		{
			m_FoldingText = text ;
		}
		
		public void clear()
		{
			m_TextBlocks.clear() ;
			m_LastBlock = null ;
		}
		
		public String toString()
		{
			StringBuffer buffer = new StringBuffer() ;
			
			buffer.append(m_TextBlocks.toString()) ;
			
			return buffer.toString() ;
		}
		
		public void addBlock(Block block)
		{
			block.setIndex(m_TextBlocks.size()) ;
			int start = m_LastBlock != null ? m_LastBlock.getStart() + m_LastBlock.getVisibleSize() : 0 ;
			block.setStart(start) ;
			
			m_TextBlocks.add(block) ;
			m_LastBlock = block ;
		}
		
		/** Returns the block (if any) which starts at the given line number. */
		public Block getBlockStartsAtLineNumber(int lineNumber)
		{
			int size = m_TextBlocks.size() ;
			for (int b = 0 ; b < size ; b++)
			{
				// NOTE: Depending on how often this is called we could hash these values
				Block block = (Block)m_TextBlocks.get(b) ;
				if (block.getStart() == lineNumber)
					return block ;
				
				// As soon as we reach a block after the target line we know we have
				// no match
				if (block.getStart() > lineNumber)
					return null ;
			}
			
			return null ;
		}
		
		/** Returns the index of the first block which starts at lineNumber or greater.  -1 if none. */
		public int getBlockAfterLineNumber(int lineNumber)
		{
			// NOTE: Depending on how often this is called we could hash these values
			int size = m_TextBlocks.size() ;
			for (int b = 0 ; b < size ; b++)
			{
				Block block = (Block)m_TextBlocks.get(b) ;
				if (block.getStart() >= lineNumber)
					return b ;
			}
			
			return -1 ;
		}
		
		public Block getBlock(int index)
		{
			return (Block)m_TextBlocks.get(index) ;
		}
		
		public int getNumberBlocks()
		{
			return m_TextBlocks.size() ;
		}
		
		/** Returns the character positions for the start and end of a block -- so we can use these to set the selection to the block */
		public Point getBlockSelectionRange(Block block)
		{
			int start = 0 ;

			for (int b = 0 ; b < block.getIndex() ; b++)
			{
				int chars = ((Block)m_TextBlocks.get(b)).getVisibleCharCount() ;
				start += chars ;
			}
			
			int end = start + block.getVisibleCharCount() ;
			
			return new Point(start, end) ;
		}
		
		public void expandBlock(Block block, boolean state)
		{
			if (block.isExpanded() == state || !block.canExpand() || block.getSize() == 1)
				return ;

			Point range = getBlockSelectionRange(block) ;
			int delta = 0 ;
			
			//boolean selected = (m_FoldingText.m_Text.getSelectionCount() > 1) ;
			
			m_FoldingText.m_Text.setSelection(range) ;
			
			// For debugging show selection and then update it
			//if (!selected)
			//	return ;
			
			if (state)
			{
				// Expanding
				m_FoldingText.m_Text.insert(block.getAll()) ;
				delta = block.getSize() - 1 ;
			}
			else
			{
				m_FoldingText.m_Text.insert(block.getFirst()) ;
				delta = 1 - block.getSize() ;
			}
			
			// Update the remaining block position info
			int size = getNumberBlocks() ;
			for (int b = block.getIndex()+1 ; b < size ; b++)
			{
				Block update = getBlock(b) ;
				update.setStart(update.getStart() + delta) ;
			}
			
			block.setExpand(state) ;
		}
	}
	
	/** Represents a section of text that is a single unit for expanding/collapsing. */
	public static class Block
	{
		protected int		m_Index ;
		protected boolean	m_CanExpand ;
		protected boolean	m_IsExpanded ;
		protected int		m_Start ;				// The first line where this block appears in the text widget

		protected ArrayList m_Lines = new ArrayList() ;
		protected StringBuffer m_All = new StringBuffer() ;
		
		public Block(boolean canExpand) { m_CanExpand = canExpand ; m_IsExpanded = false ; }
		
		public void setIndex(int index)	{ m_Index = index ; }
		public int getIndex()			{ return m_Index ; }
		
		public void setStart(int start)			{ m_Start = start ; }
		public int  getStart() 					{ return m_Start ; }
		public void setCanExpand(boolean state)	{ m_CanExpand = state ; }
		public boolean canExpand()				{ return m_CanExpand && m_Lines.size() > 1 ; }
		public void setExpand(boolean state)	{ m_IsExpanded = state ; }
		
		// Blocks which can't expand/collapse are always in the expanded state
		public boolean isExpanded()				{ return m_IsExpanded || !m_CanExpand ; }
		
		public int  getSize()  				{ return m_Lines.size() ; }
		public int  getVisibleSize()		{ return isExpanded() ? m_Lines.size() : 1 ; }
		public void appendLine(String text) { m_Lines.add(text) ; m_All.append(text) ; }
		public void removeLastLine()		{ m_Lines.remove(m_Lines.size() - 1) ; recalcAll() ; if (m_Lines.size() == 0) throw new IllegalStateException("Shouldn't empty a block") ; }
		
		public String getFirst() 			{ return (String)m_Lines.get(0) ; }
		public String getAll()				{ return m_All.toString() ; }
		public String getLastLine()			{ if (m_Lines.size() == 0) throw new IllegalStateException("Block shouldn't be empty") ;
											  return (String)m_Lines.get(m_Lines.size()-1) ; }
		private void recalcAll()
		{
			m_All = new StringBuffer() ;
			
			for (int i = 0 ; i < m_Lines.size() ; i++)
				m_All.append((String)m_Lines.get(i)) ;
		}
		
		public int getFirstLineCharCount()	{ return getFirst().length() + 1 ; }			// Add one because \n in text becomes \r\n in Text control (BUGBUG: Windows only?)
		public int getAllCharCount()		{ return m_All.length() + m_Lines.size() ; }	// Have to add one newline char per line to make this correct (as above)
		public int getVisibleCharCount()	{ return isExpanded() ? getAllCharCount() : getFirstLineCharCount() ; }
		
		public String toString()
		{
			StringBuffer buffer = new StringBuffer() ;
			
			buffer.append("(") ;
			buffer.append(m_Start) ;
			buffer.append(m_CanExpand ? (m_IsExpanded ? "-" : "+") : "!") ;
			buffer.append(",") ;
			buffer.append(" Size ") ;
			buffer.append(getSize()) ;
			buffer.append(")") ;
			
			return buffer.toString() ;
		}
	}
	
	public FoldingText(Composite parent)
	{
		m_Container = new Composite(parent, 0) ;
		m_IconBar	= new Canvas(m_Container, 0) ;
		m_Text      = new Text(m_Container, SWT.MULTI | SWT.V_SCROLL | SWT.READ_ONLY) ;
		
		GridLayout layout = new GridLayout() ;
		layout.numColumns = 2 ;
		m_Container.setLayout(layout) ;
		
		GridData data1 = new GridData(GridData.FILL_VERTICAL) ;
		data1.widthHint = 15 ;
		m_IconBar.setLayoutData(data1) ;

		GridData data2 = new GridData(GridData.FILL_BOTH) ;
		m_Text.setLayoutData(data2) ;
		
		// Canned data for testing
		/*
		int lineCount = 0 ;
		for (int b = 0 ; b < 3 ; b++)
		{
			Block block = new Block(true) ;
			block.setStart(lineCount) ;
			
			for (int i = 0 ; i < 50 ; i++)
			{		
				lineCount++ ;
				String line = "This is line " + i + " in block " + b + "\n" ;
				block.appendLine(line) ;
			}
			m_Text.append(block.getAll()) ;
			block.setExpand(true) ;
			
			m_FoldingDoc.addBlock(block) ;
		}
		*/
		
		m_IconBar.addPaintListener(new PaintListener() { public void paintControl(PaintEvent e) { paintIcons(e) ; } } ) ;
		m_IconBar.setBackground(m_IconBar.getDisplay().getSystemColor(SWT.COLOR_WHITE)) ;

		m_IconBar.addMouseListener(new MouseAdapter() { public void mouseUp(MouseEvent e) { iconBarMouseClick(e) ; } } ) ;
		
		// Think we'll need this so we update the icons while we're scrolling
		m_Text.getVerticalBar().addSelectionListener(new SelectionAdapter() { public void widgetSelected(SelectionEvent e) { m_IconBar.redraw() ; } }) ;
	}
	
	/* Snippet code to show how to detect any scroll, not just the thumb wheel.
	Display display = new Display ();
	Shell shell = new Shell (display);
	shell.setLayout (new FillLayout ());
	final Text text = new Text (shell, SWT.BORDER | SWT.H_SCROLL | SWT.V_SCROLL);
	for (int i=0; i<32; i++) {
		text.append (i + "-This is a line of text in a widget-" + i + "\n");
	}
	text.setSelection (0);
	Listener listener = new Listener () {
		int lastIndex = text.getTopIndex ();
		public void handleEvent (Event e) {
			int index = text.getTopIndex ();
			if (index != lastIndex) {
				lastIndex = index;
				System.out.println ("Scrolled, topIndex=" + index);
			}
		}
	};
	// NOTE: Only detects scrolling by the user
	text.addListener (SWT.MouseDown, listener);
	text.addListener (SWT.MouseMove, listener);
	text.addListener (SWT.MouseUp, listener);
	text.addListener (SWT.KeyDown, listener);
	text.addListener (SWT.KeyUp, listener);
	text.addListener (SWT.Resize, listener);
	ScrollBar hBar = text.getHorizontalBar();
	if (hBar != null) {
		hBar.addListener (SWT.Selection, listener);
	}
	ScrollBar vBar = text.getVerticalBar();
	if (vBar != null) {
		vBar.addListener (SWT.Selection, listener);
	}
	shell.pack ();
	Point size = shell.computeSize (SWT.DEFAULT, SWT.DEFAULT);
	shell.setSize (size. x - 32, size.y / 2);
	shell.open ();
	while (!shell.isDisposed ()) {
		if (!display.readAndDispatch ()) display.sleep ();
	}
	display.dispose ();
	*/
	
	public void clear()
	{
		m_FoldingDoc.clear() ;
		m_Text.setText("") ;
		m_IconBar.redraw() ;
	}
	
	public String toString()
	{
		return m_FoldingDoc.toString() ;
	}
	
	public void appendText(String text, boolean redraw)
	{
		Block last = m_FoldingDoc.m_LastBlock ;
		
		if (last == null || last.canExpand())
		{
			// We create blocks that can't fold to hold top level text lines
			last = new Block(false) ;
			m_FoldingDoc.addBlock(last) ;
		}
		
		last.appendLine(text) ;
		m_Text.append(text) ;
	}
	
	public void appendSubText(String text, boolean redraw)
	{
		Block last = m_FoldingDoc.m_LastBlock ;
		
		if (last == null)
		{
			// This is an odd situation where we're adding subtext but have no supertext to append to.
			// It probably will never occur, but if it does we'll add a blank super text block just
			// to get us going.
			last = new Block(true) ;
			last.appendLine("") ;
			m_FoldingDoc.addBlock(last) ;			
			m_Text.append("") ;
		}

		if (!last.canExpand())
		{
			// Need to remove the last line from the block and move it to a new block
			// which can expand and then proceed with the addition to this new block
			// Thus we ensure that "canExpand" items have at least one child.
			int size = last.getSize() ;
			
			if (size == 1)
			{
				// If the last block only contains exactly one line we can convert it safely
				// to a block which does expand.  This way we also preserve the "all blocks contain
				// at least one line" rule which makes the code simpler.
				last.setCanExpand(true) ;
			}
			else if (size > 1)
			{
				// NOTE: Blocks should always have at least one line so these calls
				// should never fail (if they do it's a programming error somewhere else that
				// allowed an empty block to be created).
				String lastLine = last.getLastLine() ;
				last.removeLastLine() ;
				
				last = new Block(true) ;
				last.appendLine(lastLine) ;
				m_FoldingDoc.addBlock(last) ;
			} else if (size == 0)
			{
				throw new IllegalStateException("Last block should not be empty") ;
			}
			
			// There's no change to the text sprite (because the line is just moved between logical blocks)
			// but we will need to draw the icons to show there's a new block.
			m_IconBar.redraw() ;
		}
		
		last.appendLine(text) ;

		if (last.isExpanded())
			m_Text.append(text) ;
	}
	
	public Composite getWindow() 	 { return m_Container ; }
	public Text      getTextWindow() { return m_Text ; }
	
	protected void iconBarMouseClick(MouseEvent e)
	{
		// Make sure the text control is properly initialized
		if (m_Text.getLineHeight() == 0)
			return ;

		int topLine = m_Text.getTopIndex() ;
		int lineHeight = m_Text.getLineHeight() ;

		int line = (e.y / lineHeight) + topLine ;
		
		Block block = m_FoldingDoc.getBlockStartsAtLineNumber(line) ;
		
		if (block == null)
			return ;
		
		m_FoldingDoc.expandBlock(block, !block.isExpanded()) ;
		m_IconBar.redraw() ;
	}
	
	protected void paintIcons(PaintEvent e)
	{
		GC gc = e.gc;
		
		int scrollPosition = m_Text.getVerticalBar().getSelection() ;
		
		Canvas canvas = m_IconBar ;
		Rectangle client = canvas.getClientArea ();

		// Make sure the text control is properly initialized
		if (m_Text.getLineHeight() == 0)
			return ;

		// Get all the information about which part of the text window is visible
		int topLine = m_Text.getTopIndex() ;
		int lineHeight = m_Text.getLineHeight() ;
		int visibleLines = m_Text.getClientArea().height / lineHeight ;
		int lastLine = Math.min(m_Text.getLineCount(),m_Text.getTopIndex() + visibleLines) ;
		
		// Start with the first block that appears at or after "topLine"
		int blockIndex = m_FoldingDoc.getBlockAfterLineNumber(topLine) ;
		int blockCount = m_FoldingDoc.getNumberBlocks() ;

		int outerSize = 11 ;
		int innerSize = 7 ;
		int offset1 = (client.width - outerSize) / 2 ;
		int offset2 = (client.width - innerSize) / 2 ;
		
		Color gray  = canvas.getDisplay().getSystemColor(SWT.COLOR_GRAY) ;
		Color black = canvas.getDisplay().getSystemColor(SWT.COLOR_BLACK) ;

		// Go through each block in turn until we're off the bottom of the screen
		// or at the end of the list of blocks drawing icons
		while (blockIndex != -1 && blockIndex < blockCount)
		{
			Block block = m_FoldingDoc.getBlock(blockIndex) ;
			
			int line = block.getStart() ;
			
			// Once we drop off the bottom of the screen we're done
			if (line >= lastLine)
				break ;
		
			int pos = line - topLine ;
			int y = pos * lineHeight ;
				
			boolean expanded = block.isExpanded() ;
			
			if (block.canExpand())
			{
				//gc.setForeground(gray) ;
				gc.drawRectangle(offset1, y + offset1, outerSize-1, outerSize-1) ;
				//gc.setForeground(black) ;
				
				// + if collapsed
				// - if expanded
				int y1 = y + lineHeight/2 - 1;
				gc.drawLine(offset2, y1, offset2 + innerSize-1, y1) ;

				if (!expanded)
				{
					int x1 = client.width / 2 ;
					gc.drawLine(x1, y + offset2, x1, y + offset2 + innerSize-1) ;
				}				
			}
			blockIndex++ ;				
		}
	}
}
