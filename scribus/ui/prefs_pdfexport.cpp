/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "ui/prefs_pdfexport.h"
#include "ui/createrange.h"
#include "prefsstructs.h"

#include "scribuscore.h"
#include "scribusdoc.h"
#include "ui/scrspinbox.h"
#include "units.h"
#include "util.h"
#include "util_icon.h"

Prefs_PDFExport::Prefs_PDFExport(QWidget* parent, ScribusDoc* doc)
	: Prefs_Pane(parent),
	cmsEnabled(false),
	m_doc(doc),
	exportingPDF(false)
{
	setupUi(this);
	pageMirrorHorizontalToolButton->setIcon(QIcon(loadIcon("16/flip-object-horizontal.png")));
	pageMirrorVerticalToolButton->setIcon(QIcon(loadIcon("16/flip-object-vertical.png")));
	unitChange(0);
	languageChange();

	connect(exportChosenPagesRadioButton, SIGNAL(toggled(bool)), this, SLOT(enableRangeControls(bool)));
	connect(exportRangeMorePushButton, SIGNAL(clicked()), this, SLOT(createPageNumberRange()));
	connect(maxResolutionLimitCheckBox, SIGNAL(clicked()), this, SLOT(setMaximumResolution()));
//	connect(MirrorH, SIGNAL(clicked()), this, SLOT(PDFMirror()));
//	connect(MirrorV, SIGNAL(clicked()), this, SLOT(PDFMirror()));
//	connect(RotateDeg, SIGNAL(activated(int)), this, SLOT(Rotation(int)));
	connect(outputIntentionComboBox, SIGNAL(activated(int)), this, SLOT(enableProfiles(int)));
	connect(useSolidColorProfileCheckBox, SIGNAL(clicked()), this, SLOT(enablePG()));
	connect(useImageProfileCheckBox, SIGNAL(clicked()), this, SLOT(enablePGI()));
	connect(doNotUseEmbeddedImageProfileCheckBox, SIGNAL(clicked()), this, SLOT(enablePGI2()));
	connect(pdfVersionComboBox, SIGNAL(activated(int)), this, SLOT(enablePDFX(int)));
	connect(useEncryptionCheckBox, SIGNAL(clicked(bool)), this, SLOT(enableSecurityControls(bool)));
}

Prefs_PDFExport::~Prefs_PDFExport()
{
}


void Prefs_PDFExport::unitChange(int unitIndex)
{
	unitRatio = unitGetRatioFromIndex(unitIndex);
	registrationMarkOffsetSpinBox->setNewUnit(unitIndex);
	registrationMarkOffsetSpinBox->setMinimum(0);
	registrationMarkOffsetSpinBox->setMaximum(3000 * unitRatio);
}

void Prefs_PDFExport::languageChange()
{
	int i=0;
	i = rotationComboBox->currentIndex();
	rotationComboBox->clear();
	rotationComboBox->addItem(QString::fromUtf8("0 °"));
	rotationComboBox->addItem(QString::fromUtf8("90 °"));
	rotationComboBox->addItem(QString::fromUtf8("180 °"));
	rotationComboBox->addItem(QString::fromUtf8("270 °"));
	rotationComboBox->setCurrentIndex(i);

	i = imageCompressionMethodComboBox->currentIndex();
	imageCompressionMethodComboBox->clear();
	imageCompressionMethodComboBox->addItem( tr( "Automatic" ) );
	imageCompressionMethodComboBox->addItem( tr( "Lossy - JPEG" ) );
	imageCompressionMethodComboBox->addItem( tr( "Lossless - Zip" ) );
	imageCompressionMethodComboBox->addItem( tr( "None" ) );
	imageCompressionMethodComboBox->setCurrentIndex(i);

	i = imageCompressionQualityComboBox->currentIndex();
	imageCompressionQualityComboBox->clear();
	imageCompressionQualityComboBox->addItem( tr( "Maximum" ) );
	imageCompressionQualityComboBox->addItem( tr( "High" ) );
	imageCompressionQualityComboBox->addItem( tr( "Medium" ) );
	imageCompressionQualityComboBox->addItem( tr( "Low" ) );
	imageCompressionQualityComboBox->addItem( tr( "Minimum" ) );
	imageCompressionQualityComboBox->setCurrentIndex(i);

	addPDFVersions(true);//if (cmsEnabled)// && (!PDFXProfiles.isEmpty()))

	i = pageBindingComboBox->currentIndex();
	pageBindingComboBox->clear();
	pageBindingComboBox->addItem( tr("Left Margin"));
	pageBindingComboBox->addItem( tr("Right Margin"));
	pageBindingComboBox->setCurrentIndex(i);

	i = outputIntentionComboBox->currentIndex();
	outputIntentionComboBox->clear();
	outputIntentionComboBox->addItem( tr( "Screen / Web" ) );
	outputIntentionComboBox->addItem( tr( "Printer" ) );
	outputIntentionComboBox->addItem( tr( "Grayscale" ) );
	outputIntentionComboBox->setCurrentIndex(i);

	i = customRenderingSpotFunctionComboBox->currentIndex();
	customRenderingSpotFunctionComboBox->clear();
	customRenderingSpotFunctionComboBox->addItem( tr( "Simple Dot" ) );
	customRenderingSpotFunctionComboBox->addItem( tr( "Line" ) );
	customRenderingSpotFunctionComboBox->addItem( tr( "Round" ) );
	customRenderingSpotFunctionComboBox->addItem( tr( "Ellipse" ) );
	customRenderingSpotFunctionComboBox->setCurrentIndex(i);

	i = solidColorRenderingIntentComboBox->currentIndex();
	int j=imageRenderingIntentComboBox->currentIndex();
	solidColorRenderingIntentComboBox->clear();
	imageRenderingIntentComboBox->clear();
	QString tmp_ip[] = { tr("Perceptual"), tr("Relative Colorimetric"), tr("Saturation"), tr("Absolute Colorimetric")};
	size_t ar_ip = sizeof(tmp_ip) / sizeof(*tmp_ip);
	for (uint a = 0; a < ar_ip; ++a)
		solidColorRenderingIntentComboBox->addItem(tmp_ip[a]);
	for (uint a = 0; a < ar_ip; ++a)
		imageRenderingIntentComboBox->addItem(tmp_ip[a]);
	solidColorRenderingIntentComboBox->setCurrentIndex(i);
	imageRenderingIntentComboBox->setCurrentIndex(j);
}

void Prefs_PDFExport::restoreDefaults(struct ApplicationPrefs *prefsData)
{
}

void Prefs_PDFExport::restoreDefaults(struct ApplicationPrefs *prefsData, const ProfilesL & PDFXProfiles, bool exporting)
{
	exportingPDF=exporting;
	enablePDFExportTabs(exportingPDF);
	AllFonts=prefsData->fontPrefs.AvailFonts;
	int unitIndex = prefsData->docSetupPrefs.docUnitIndex;
	unitRatio = unitGetRatioFromIndex(unitIndex);
	unitChange(unitIndex);
	Opts=prefsData->pdfPrefs;
	defaultSolidColorRGBProfile=prefsData->colorPrefs.DCMSset.DefaultSolidColorRGBProfile;
	defaultPrinterProfile=prefsData->colorPrefs.DCMSset.DefaultPrinterProfile;
	exportAllPagesRadioButton->setChecked(true);
	enableRangeControls(false);
	rotationComboBox->setCurrentIndex(prefsData->pdfPrefs.RotateDeg / 90);
	pageMirrorHorizontalToolButton->setChecked(prefsData->pdfPrefs.MirrorH);
	pageMirrorVerticalToolButton->setChecked(prefsData->pdfPrefs.MirrorV);
	clipToPrinterMarginsCheckBox->setChecked(prefsData->pdfPrefs.doClip);
	bool cmsUse = m_doc ? (ScCore->haveCMS() && m_doc->HasCMS) : false;
	int newCMSIndex=0;
	if (cmsUse)
	{
		if (prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_X1a)
			newCMSIndex=3;
		if (prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_X3)
			newCMSIndex=4;
		if (prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_X4)
			newCMSIndex=5;
	}
	if (prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_13)
		newCMSIndex=0;
	if (prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_14)
		newCMSIndex=1;
	if (prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_15)
		newCMSIndex=2;
	pdfVersionComboBox->setCurrentIndex(newCMSIndex);
	pageBindingComboBox->setCurrentIndex(prefsData->pdfPrefs.Binding);
	generateThumbnailsCheckBox->setChecked(prefsData->pdfPrefs.Thumbnails);
	saveLinkedTextFramesAsArticlesCheckBox->setChecked(prefsData->pdfPrefs.Articles);
	includeBookmarksCheckBox->setChecked(prefsData->pdfPrefs.Bookmarks);
	includeLayersCheckBox->setChecked(prefsData->pdfPrefs.useLayers);
	includeLayersCheckBox->setEnabled(prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_15 || prefsData->pdfPrefs.Version == PDFOptions::PDFVersion_X4);

	epsExportResolutionSpinBox->setValue(prefsData->pdfPrefs.Resolution);
	embedPDFAndEPSFiles->setChecked(prefsData->pdfPrefs.embedPDF);
	compressTextAndVectorGraphicsCheckBox->setChecked( prefsData->pdfPrefs.Compress );
	imageCompressionMethodComboBox->setCurrentIndex(prefsData->pdfPrefs.CompressMethod);
	imageCompressionQualityComboBox->setCurrentIndex(prefsData->pdfPrefs.Quality);
	maxResolutionLimitCheckBox->setChecked(prefsData->pdfPrefs.RecalcPic);
	maxExportResolutionSpinBox->setValue(prefsData->pdfPrefs.PicRes);
	maxExportResolutionSpinBox->setEnabled(prefsData->pdfPrefs.RecalcPic);

	if (m_doc != 0 && exportingPDF)
	{
//	Build a list of all Fonts used in Annotations;
		PageItem *pgit;
		for (int c = 0; c < m_doc->FrameItems.count(); ++c)
		{
			pgit=m_doc->FrameItems.at(c);
			if (((pgit->itemType() == PageItem::TextFrame) || (pgit->itemType() == PageItem::PathText)) && (pgit->isAnnotation()) && (pgit->itemText.length() > 0))
				AnnotationFonts.insert(pgit->itemText.defaultStyle().charStyle().font().replacementName(), "");
		}
		for (int c = 0; c < m_doc->MasterItems.count(); ++c)
		{
			pgit=m_doc->MasterItems.at(c);
			if (((pgit->itemType() == PageItem::TextFrame) || (pgit->itemType() == PageItem::PathText)) && (pgit->isAnnotation()) && (pgit->itemText.length() > 0))
				AnnotationFonts.insert(pgit->itemText.defaultStyle().charStyle().font().replacementName(), "");
		}
		for (int c = 0; c < m_doc->DocItems.count(); ++c)
		{
			pgit=m_doc->DocItems.at(c);
			if (((pgit->itemType() == PageItem::TextFrame) || (pgit->itemType() == PageItem::PathText)) && (pgit->isAnnotation()) && (pgit->itemText.length() > 0))
				AnnotationFonts.insert(pgit->itemText.defaultStyle().charStyle().font().replacementName(), "");
		}
		QMap<QString,int>::const_iterator it;
		availableFontsListWidget->clear();
		for (it = m_doc->usedFonts().constBegin(); it != m_doc->usedFonts().constEnd(); ++it)
		{
			if (AllFonts[it.key()].isReplacement())
				new QListWidgetItem( QIcon(loadIcon("font_subst16.png")), it.key(), availableFontsListWidget );
			else if (AllFonts[it.key()].type() == ScFace::TYPE1)
				new QListWidgetItem( QIcon(loadIcon("font_type1_16.png")), it.key(), availableFontsListWidget );
			else if (AllFonts[it.key()].type() == ScFace::TTF)
				new QListWidgetItem( QIcon(loadIcon("font_truetype16.png")), it.key(), availableFontsListWidget );
			else if (AllFonts[it.key()].type() == ScFace::OTF)
				new QListWidgetItem( QIcon(loadIcon("font_otf16.png")), it.key(), availableFontsListWidget );
		}
		toEmbedButton->setEnabled(false);
		fromEmbedButton->setEnabled(false);
		toOutlineButton->setEnabled(false);
		fromOutlineButton->setEnabled(false);
		if ((Opts.EmbedList.count() == 0) && (Opts.SubsetList.count() == 0) && (Opts.firstUse))
			EmbedAll();
		else
		{
			embeddedFontsListWidget->clear();
			FontsToEmbed.clear();
			for (int fe = 0; fe < Opts.EmbedList.count(); ++fe)
			{
				embeddedFontsListWidget->addItem(Opts.EmbedList[fe]);
				FontsToEmbed.append(Opts.EmbedList[fe]);
			}
			if (Opts.SubsetList.count() != 0)
			{
				outlinedFontsListWidget->clear();
				FontsToOutline.clear();
				for (int fe = 0; fe < Opts.SubsetList.count(); ++fe)
				{
					outlinedFontsListWidget->addItem(Opts.SubsetList[fe]);
					FontsToOutline.append(Opts.SubsetList[fe]);
				}
			}
			QMap<QString, QString>::Iterator itAnn;
			for (itAnn = AnnotationFonts.begin(); itAnn != AnnotationFonts.end(); ++itAnn)
			{
				if (FontsToEmbed.contains(itAnn.key()) == 0)
				{
					embeddedFontsListWidget->addItem(itAnn.key());
					embeddedFontsListWidget->item(embeddedFontsListWidget->count()-1)->setFlags(Qt::ItemIsEnabled);
					FontsToEmbed.append(itAnn.key());
				}
				if (FontsToOutline.contains(itAnn.key()) != 0)
				{
					FontsToOutline.removeAll(itAnn.key());
					QList<QListWidgetItem *> itR = outlinedFontsListWidget->findItems(itAnn.key(), Qt::MatchExactly);
					delete outlinedFontsListWidget->takeItem(outlinedFontsListWidget->row(itR.at(0)));
				}
			}
		}
		enabledEffectsCheckBox->setChecked(Opts.PresentMode);
		showPagePreviewsCheckBox->setChecked(false);
		effectsPageListWidget->clear();
		QString tmp;
		struct PDFPresentationData ef;
		if (EffVal.count() != 0)
		{
			for (int pg2 = 0; pg2 < m_doc->Pages->count(); ++pg2)
			{
				effectsPageListWidget->addItem( tr("Page")+" "+tmp.setNum(pg2+1));
				if (EffVal.count()-1 < pg2)
				{
					ef.pageEffectDuration = 1;
					ef.pageViewDuration = 1;
					ef.effectType = 0;
					ef.Dm = 0;
					ef.M = 0;
					ef.Di = 0;
					EffVal.append(ef);
				}
			}
		}
		else
		{
			for (int pg = 0; pg < m_doc->Pages->count(); ++pg)
			{
				effectsPageListWidget->addItem( tr("Page")+" "+tmp.setNum(pg+1));
				ef.pageEffectDuration = 1;
				ef.pageViewDuration = 1;
				ef.effectType = 0;
				ef.Dm = 0;
				ef.M = 0;
				ef.Di = 0;
				EffVal.append(ef);
			}
		}
		displayDurationSpinBox->setValue(EffVal[0].pageViewDuration);
		effectDurationSpinBox->setValue(EffVal[0].pageEffectDuration);
		bool df = true;
		if ((Opts.displayBookmarks) || (Opts.displayFullscreen) || (Opts.displayLayers) || (Opts.displayThumbs))
			df = false;
		if (df)
			useViewerDefaultsRadioButton->setChecked(df);
		useFullScreenRadioButton->setChecked(Opts.displayFullscreen);
		useBookmarksRadioButton->setChecked(Opts.displayBookmarks);
		useThumbnailsRadioButton->setChecked(Opts.displayThumbs);
		useLayersRadioButton->setChecked(Opts.displayLayers);
		hideViewerToolBarCheckBox->setChecked(Opts.hideToolBar);
		hideViewerMenuBarCheckBox->setChecked(Opts.hideMenuBar);
		fitViewerWindowCheckBox->setChecked(Opts.fitWindow);
		QMap<QString,QString>::Iterator itja;
		startupJavascriptComboBox->clear();
		startupJavascriptComboBox->addItem( tr("No Script"));
		for (itja = m_doc->JavaScripts.begin(); itja != m_doc->JavaScripts.end(); ++itja)
			startupJavascriptComboBox->addItem(itja.key());
		if (m_doc->JavaScripts.contains(Opts.openAction))
			setCurrentComboItem(startupJavascriptComboBox, Opts.openAction);
		if (Opts.PageLayout == PDFOptions::SinglePage)
			singlePageRadioButton->setChecked(true);
		else if (Opts.PageLayout == PDFOptions::OneColumn)
			continuousPagesRadioButton->setChecked(true);
		else if (Opts.PageLayout == PDFOptions::TwoColumnLeft)
			doublePageLeftRadioButton->setChecked(true);
		else if (Opts.PageLayout == PDFOptions::TwoColumnRight)
			doublePageRightRadioButton->setChecked(true);
		if ((Opts.Version == PDFOptions::PDFVersion_15) || (Opts.Version == PDFOptions::PDFVersion_X4))
			useLayersRadioButton->setEnabled(true);
		else
			useLayersRadioButton->setEnabled(false);
	}

	useEncryptionCheckBox->setChecked( prefsData->pdfPrefs.Encrypt );
	passwordOwnerLineEdit->setText(prefsData->pdfPrefs.PassOwner);
	passwordUserLineEdit->setText(prefsData->pdfPrefs.PassUser);
	allowPrintingCheckBox->setChecked( prefsData->pdfPrefs.Permissions & 4 );
	allowChangingCheckBox->setChecked( prefsData->pdfPrefs.Permissions & 8 );
	allowCopyingCheckBox->setChecked( prefsData->pdfPrefs.Permissions & 16 );
	allowAnnotatingCheckBox->setChecked( prefsData->pdfPrefs.Permissions & 32 );
	enableSecurityControls(prefsData->pdfPrefs.Encrypt);

	if (prefsData->pdfPrefs.UseRGB)
		outputIntentionComboBox->setCurrentIndex(0);
	else
	{
		if (prefsData->pdfPrefs.isGrayscale)
			outputIntentionComboBox->setCurrentIndex(2);
		else
			outputIntentionComboBox->setCurrentIndex(1);
	}
	convertSpotsToProcessCheckBox->setChecked(!prefsData->pdfPrefs.UseSpotColors);

	bleedsWidget->setup(prefsData->pdfPrefs.bleeds, prefsData->docSetupPrefs.pagePositioning, prefsData->docSetupPrefs.docUnitIndex, false, false);
	bleedsWidget->setPageWidth(prefsData->docSetupPrefs.pageWidth);
	bleedsWidget->setPageHeight(prefsData->docSetupPrefs.pageHeight);
	bleedsWidget->setPageSize(prefsData->docSetupPrefs.pageSize);
	bleedsWidget->setMarginPreset(prefsData->docSetupPrefs.marginPreset);
//
	useCustomRenderingCheckBox->setChecked(prefsData->pdfPrefs.UseLPI);
	QMap<QString,LPIData>::Iterator itlp;
	customRenderingColorComboBox->clear();
	for (itlp = prefsData->pdfPrefs.LPISettings.begin(); itlp != prefsData->pdfPrefs.LPISettings.end(); ++itlp)
		customRenderingColorComboBox->addItem( itlp.key() );
	customRenderingColorComboBox->setCurrentIndex(0);

	customRenderingFrequencySpinBox->setValue(prefsData->pdfPrefs.LPISettings[customRenderingColorComboBox->currentText()].Frequency);
	customRenderingAngleSpinBox->setValue(prefsData->pdfPrefs.LPISettings[customRenderingColorComboBox->currentText()].Angle);
	customRenderingSpotFunctionComboBox->setCurrentIndex(prefsData->pdfPrefs.LPISettings[customRenderingColorComboBox->currentText()].SpotFunc);
	useSolidColorProfileCheckBox->setChecked(prefsData->pdfPrefs.UseProfiles);
	useImageProfileCheckBox->setChecked(prefsData->pdfPrefs.UseProfiles2);
	doNotUseEmbeddedImageProfileCheckBox->setChecked(prefsData->pdfPrefs.EmbeddedI);
	if ((prefsData->pdfPrefs.UseRGB) || (prefsData->pdfPrefs.isGrayscale))
	{
		//enableSolidsImagesWidgets(false);
		enableProfiles(0);
	}
	else
		enableProfiles(1);
	enablePG();
	enablePGI();
	QString tp(prefsData->pdfPrefs.SolidProf);
	if (!ScCore->InputProfiles.contains(tp))
	{
		if (m_doc != 0 && exportingPDF)
			tp = m_doc->cmsSettings().DefaultSolidColorRGBProfile;
		else
			tp = defaultSolidColorRGBProfile;
	}
	ProfilesL::Iterator itp;
	ProfilesL::Iterator itpend=ScCore->InputProfiles.end();
	solidColorProfileComboBox->clear();
	for (itp = ScCore->InputProfiles.begin(); itp != itpend; ++itp)
	{
		solidColorProfileComboBox->addItem(itp.key());
		if (itp.key() == tp)
		{
			if (cmsUse)
				solidColorProfileComboBox->setCurrentIndex(solidColorProfileComboBox->count()-1);
		}
	}
	if (cmsUse)
		solidColorRenderingIntentComboBox->setCurrentIndex(Opts.Intent);
	QString tp1 = Opts.ImageProf;
	if (!ScCore->InputProfiles.contains(tp1))
	{
		if (m_doc != 0 && exportingPDF)
			tp1 = m_doc->cmsSettings().DefaultSolidColorRGBProfile;
		else
			tp1 = defaultSolidColorRGBProfile;
	}
	ProfilesL::Iterator itp2;
	ProfilesL::Iterator itp2end=ScCore->InputProfiles.end();
	imageProfileComboBox->clear();
	for (itp2 = ScCore->InputProfiles.begin(); itp2 != itp2end; ++itp2)
	{
		imageProfileComboBox->addItem(itp2.key());
		if (itp2.key() == tp1)
		{
			if (cmsUse)
				imageProfileComboBox->setCurrentIndex(imageProfileComboBox->count()-1);
		}
	}
	if (cmsUse)
		imageRenderingIntentComboBox->setCurrentIndex(Opts.Intent2);
	if (!cmsUse)
	{
		//Disabling vis hiding
		//setSolidsImagesWidgetsShown(false);
		enableSolidsImagesWidgets(false);
	}

	ProfilesL::const_iterator itp3;
	QString tp3(Opts.PrintProf);
	if (!PDFXProfiles.contains(tp3))
	{
		if (m_doc != 0 && exportingPDF)
			tp3 = m_doc->cmsSettings().DefaultPrinterProfile;
		else
			tp3 = defaultPrinterProfile;
	}
	pdfx3OutputProfileComboBox->clear();
	for (itp3 = PDFXProfiles.constBegin(); itp3 != PDFXProfiles.constEnd(); ++itp3)
	{
		pdfx3OutputProfileComboBox->addItem(itp3.key());
		if (itp3.key() == tp3)
			pdfx3OutputProfileComboBox->setCurrentIndex(pdfx3OutputProfileComboBox->count()-1);
	}
	pdfx3InfoStringLineEdit->setText(Opts.Info);



	if (m_doc != 0 && exportingPDF)
	{
		useDocumentBleedsCheckBox->setChecked(Opts.useDocBleeds);
		doDocBleeds();
	}

	registrationMarkOffsetSpinBox->setValue(prefsData->pdfPrefs.markOffset*unitRatio);
	printCropMarksCheckBox->setChecked(prefsData->pdfPrefs.cropMarks);
	printBleedMarksCheckBox->setChecked(prefsData->pdfPrefs.bleedMarks);
	printRegistrationMarksCheckBox->setChecked(prefsData->pdfPrefs.registrationMarks);
	printColorBarsCheckBox->setChecked(prefsData->pdfPrefs.colorMarks);
	printPageInfoCheckBox->setChecked(prefsData->pdfPrefs.docInfoMarks);
	if (!cmsUse)
		enablePDFXWidgets(false);
	if (cmsUse && (Opts.Version == PDFOptions::PDFVersion_X1a) && (!PDFXProfiles.isEmpty()))
		enablePDFX(3);
	else if (cmsUse && (Opts.Version == PDFOptions::PDFVersion_X3) && (!PDFXProfiles.isEmpty()))
		enablePDFX(4);
	else if (cmsUse && (Opts.Version == PDFOptions::PDFVersion_X4) && (!PDFXProfiles.isEmpty()))
		enablePDFX(5);
	else
		enablePDFXWidgets(false);

	if (m_doc != 0  && exportingPDF)
	{
		effectTypeComboBox->clear();
		effectTypeComboBox->addItem( tr("No Effect"));
		effectTypeComboBox->addItem( tr("Blinds"));
		effectTypeComboBox->addItem( tr("Box"));
		effectTypeComboBox->addItem( tr("Dissolve"));
		effectTypeComboBox->addItem( tr("Glitter"));
		effectTypeComboBox->addItem( tr("Split"));
		effectTypeComboBox->addItem( tr("Wipe"));
		if (Opts.Version == PDFOptions::PDFVersion_15)
		{
			effectTypeComboBox->addItem( tr("Push"));
			effectTypeComboBox->addItem( tr("Cover"));
			effectTypeComboBox->addItem( tr("Uncover"));
			effectTypeComboBox->addItem( tr("Fade"));
		}
		PgSel = 0;
		effectsPageListWidget->setCurrentRow(0);
		SetEffOpts(0);
		effectsPageListWidget->setEnabled(false);
		enableEffects(false);
		showPagePreviewsCheckBox->setEnabled(false);
		DoEffects();
		if (enabledEffectsCheckBox->isChecked())
		{
			displayDurationSpinBox->setValue(EffVal[0].pageViewDuration);
			effectDurationSpinBox->setValue(EffVal[0].pageEffectDuration);
			effectTypeComboBox->setCurrentIndex(EffVal[0].effectType);
			effectMovingDirectionComboBox->setCurrentIndex(EffVal[0].Dm);
			effectInOutComboBox->setCurrentIndex(EffVal[0].M);
			effectDirectionComboBox->setCurrentIndex(EffVal[0].Di);
			SetEffOpts(effectTypeComboBox->currentIndex());
		}
		/* Using margin widget, is this necessary?
		if (m_doc->currentPageLayout != 0)
		{
			BleedTxt3->setText( tr( "Inside:" ) );
			BleedTxt4->setText( tr( "Outside:" ) );
		}
		*/
	}
}


void Prefs_PDFExport::saveGuiToPrefs(struct ApplicationPrefs *prefsData) const
{
	prefsData->pdfPrefs.Thumbnails = generateThumbnailsCheckBox->isChecked();
	prefsData->pdfPrefs.Compress = compressTextAndVectorGraphicsCheckBox->isChecked();
	prefsData->pdfPrefs.CompressMethod = (PDFOptions::PDFCompression) imageCompressionMethodComboBox->currentIndex();
	prefsData->pdfPrefs.Quality = imageCompressionQualityComboBox->currentIndex();
	prefsData->pdfPrefs.Resolution = epsExportResolutionSpinBox->value();
	prefsData->pdfPrefs.RecalcPic = maxResolutionLimitCheckBox->isChecked();
	prefsData->pdfPrefs.PicRes = maxExportResolutionSpinBox->value();
	prefsData->pdfPrefs.Bookmarks = includeBookmarksCheckBox->isChecked();
	prefsData->pdfPrefs.Binding = pageBindingComboBox->currentIndex();
	prefsData->pdfPrefs.MirrorH = pageMirrorHorizontalToolButton->isChecked();
	prefsData->pdfPrefs.MirrorV = pageMirrorVerticalToolButton->isChecked();
	prefsData->pdfPrefs.RotateDeg = rotationComboBox->currentIndex() * 90;
	prefsData->pdfPrefs.Articles = saveLinkedTextFramesAsArticlesCheckBox->isChecked();
	prefsData->pdfPrefs.Encrypt = useEncryptionCheckBox->isChecked();
	prefsData->pdfPrefs.UseLPI = useCustomRenderingCheckBox->isChecked();
	prefsData->pdfPrefs.UseSpotColors = !convertSpotsToProcessCheckBox->isChecked();
	prefsData->pdfPrefs.doMultiFile = false;
	prefsData->pdfPrefs.cropMarks  = printCropMarksCheckBox->isChecked();
	prefsData->pdfPrefs.bleedMarks = printBleedMarksCheckBox->isChecked();
	prefsData->pdfPrefs.registrationMarks = printRegistrationMarksCheckBox->isChecked();
	prefsData->pdfPrefs.colorMarks = printColorBarsCheckBox->isChecked();
	prefsData->pdfPrefs.docInfoMarks = printPageInfoCheckBox->isChecked();
	prefsData->pdfPrefs.markOffset = registrationMarkOffsetSpinBox->value() / unitRatio;
	prefsData->pdfPrefs.useDocBleeds = useDocumentBleedsCheckBox->isChecked();
	prefsData->pdfPrefs.bleeds=bleedsWidget->margins();
	prefsData->pdfPrefs.doClip = clipToPrinterMarginsCheckBox->isChecked();
	if (useEncryptionCheckBox->isChecked())
	{
		int Perm = -64;
		if (pdfVersionComboBox->currentIndex() == 1)
			Perm &= ~0x00240000;
		if (allowPrintingCheckBox->isChecked())
			Perm += 4;
		if (allowChangingCheckBox->isChecked())
			Perm += 8;
		if (allowCopyingCheckBox->isChecked())
			Perm += 16;
		if (allowAnnotatingCheckBox->isChecked())
			Perm += 32;
		prefsData->pdfPrefs.Permissions = Perm;
		prefsData->pdfPrefs.PassOwner = passwordOwnerLineEdit->text();
		prefsData->pdfPrefs.PassUser = passwordUserLineEdit->text();
	}
	if (pdfVersionComboBox->currentIndex() == 0)
		prefsData->pdfPrefs.Version = PDFOptions::PDFVersion_13;
	if (pdfVersionComboBox->currentIndex() == 1)
		prefsData->pdfPrefs.Version = PDFOptions::PDFVersion_14;
	if (pdfVersionComboBox->currentIndex() == 2)
		prefsData->pdfPrefs.Version = PDFOptions::PDFVersion_15;
	if (pdfVersionComboBox->currentIndex() == 3)
		prefsData->pdfPrefs.Version = PDFOptions::PDFVersion_X1a;
	if (pdfVersionComboBox->currentIndex() == 4)
		prefsData->pdfPrefs.Version = PDFOptions::PDFVersion_X3;
	if (pdfVersionComboBox->currentIndex() == 5)
		prefsData->pdfPrefs.Version = PDFOptions::PDFVersion_X4;
	if (outputIntentionComboBox->currentIndex() == 0)
	{
		prefsData->pdfPrefs.isGrayscale = false;
		prefsData->pdfPrefs.UseRGB = true;
		prefsData->pdfPrefs.UseProfiles = false;
		prefsData->pdfPrefs.UseProfiles2 = false;
	}
	else
	{
		if (outputIntentionComboBox->currentIndex() == 2)
		{
			prefsData->pdfPrefs.isGrayscale = true;
			prefsData->pdfPrefs.UseRGB = false;
			prefsData->pdfPrefs.UseProfiles = false;
			prefsData->pdfPrefs.UseProfiles2 = false;
		}
		else
		{
			prefsData->pdfPrefs.isGrayscale = false;
			prefsData->pdfPrefs.UseRGB = false;
			if (/*CMSuse*/ ScCore->haveCMS())
			{
				prefsData->pdfPrefs.UseProfiles = useSolidColorProfileCheckBox->isChecked();
				prefsData->pdfPrefs.UseProfiles2 = useImageProfileCheckBox->isChecked();
				prefsData->pdfPrefs.Intent = solidColorRenderingIntentComboBox->currentIndex();
				prefsData->pdfPrefs.Intent2 = imageRenderingIntentComboBox->currentIndex();
				prefsData->pdfPrefs.EmbeddedI = doNotUseEmbeddedImageProfileCheckBox->isChecked();
				prefsData->pdfPrefs.SolidProf = solidColorProfileComboBox->currentText();
				prefsData->pdfPrefs.ImageProf = imageProfileComboBox->currentText();
				prefsData->pdfPrefs.PrintProf = pdfx3OutputProfileComboBox->currentText();
			}
		}
	}
}

void Prefs_PDFExport::enableRangeControls(bool enabled)
{
	exportPageListLineEdit->setEnabled( enabled );
	exportRangeMorePushButton->setEnabled( enabled );
	if (enabled == false)
		includeBookmarksCheckBox->setChecked(false);
}

void Prefs_PDFExport::enableSecurityControls(bool enabled)
{
	passwordOwnerLineEdit->setEnabled(enabled);
	passwordUserLineEdit->setEnabled(enabled);
	allowPrintingCheckBox->setEnabled(enabled);
	allowChangingCheckBox->setEnabled(enabled);
	allowCopyingCheckBox->setEnabled(enabled);
	allowAnnotatingCheckBox->setEnabled(enabled);
}


void Prefs_PDFExport::enableCMS(bool enabled)
{
	cmsEnabled=enabled;
	addPDFVersions(enabled);
	enableProfiles(enabled);
}

void Prefs_PDFExport::enablePDFExportTabs(bool enabled)
{
	if (!enabled)
	{
		tabWidget->removeTab(tabWidget->indexOf(tabFonts));//Fonts
		tabWidget->removeTab(tabWidget->indexOf(tabExtras));//Extras
		tabWidget->removeTab(tabWidget->indexOf(tabViewer));//Viewer
	}
}

void Prefs_PDFExport::createPageNumberRange()
{
	if (m_doc!=0)
	{
		CreateRange cr(exportPageListLineEdit->text(), m_doc->DocPages.count(), this);
		if (cr.exec())
		{
			CreateRangeData crData;
			cr.getCreateRangeData(crData);
			exportPageListLineEdit->setText(crData.pageRange);
			return;
		}
	}
	exportPageListLineEdit->setText(QString::null);
}

void Prefs_PDFExport::setMaximumResolution()
{
	if (maxResolutionLimitCheckBox->isChecked())
	{
		maxExportResolutionSpinBox->setEnabled(true);
		if (maxExportResolutionSpinBox->value() > epsExportResolutionSpinBox->value())
			maxExportResolutionSpinBox->setValue(epsExportResolutionSpinBox->value());
	}
	else
		maxExportResolutionSpinBox->setEnabled(false);
}

void Prefs_PDFExport::enableProfiles(int i)
{
	enableLPI(i);
	bool setter = false;
	if (i == 1 && pdfVersionComboBox->currentIndex() != 3)
		setter = true;
	enableSolidsImagesWidgets(setter);
}

void Prefs_PDFExport::enableLPI(int i)
{
	if (i == 1)
	{
		QString tp(Opts.SolidProf);
		if (!ScCore->InputProfiles.contains(tp))
		{
			if (m_doc != 0)
				tp = m_doc->cmsSettings().DefaultSolidColorRGBProfile;
			else
				tp = defaultSolidColorRGBProfile;
		}
		solidColorProfileComboBox->clear();
		ProfilesL::Iterator itp;
		ProfilesL::Iterator itpend=ScCore->InputProfiles.end();
		for (itp = ScCore->InputProfiles.begin(); itp != itpend; ++itp)
		{
			solidColorProfileComboBox->addItem(itp.key());
			if (itp.key() == tp)
			{
				if (cmsEnabled)
					solidColorProfileComboBox->setCurrentIndex(solidColorProfileComboBox->count()-1);
			}
		}
		if (cmsEnabled)
			solidColorRenderingIntentComboBox->setCurrentIndex(Opts.Intent);
		QString tp1 = Opts.ImageProf;
		if (!ScCore->InputProfiles.contains(tp1))
		{
			if (m_doc != 0)
				tp1 = m_doc->cmsSettings().DefaultSolidColorRGBProfile;
			else
				tp1 = defaultSolidColorRGBProfile;
		}
		imageProfileComboBox->clear();
		ProfilesL::Iterator itp2;
		ProfilesL::Iterator itp2end=ScCore->InputProfiles.end();
		for (itp2 = ScCore->InputProfiles.begin(); itp2 != itp2end; ++itp2)
		{
			imageProfileComboBox->addItem(itp2.key());
			if (itp2.key() == tp1)
			{
				if (cmsEnabled)
					imageProfileComboBox->setCurrentIndex(imageProfileComboBox->count()-1);
			}
		}
		if (cmsEnabled)
			imageRenderingIntentComboBox->setCurrentIndex(Opts.Intent2);
		//Disabling vis hiding
		enableSolidsImagesWidgets(cmsEnabled);
		convertSpotsToProcessCheckBox->setEnabled(true);
		if (m_doc!=0)
		{
			useCustomRenderingCheckBox->show();
			enableCustomRenderingWidgets(useCustomRenderingCheckBox->isChecked());
		}
	}
	else
	{
		convertSpotsToProcessCheckBox->setEnabled(false);
		useCustomRenderingCheckBox->hide();
		enableCustomRenderingWidgets(false);
	}
}

void Prefs_PDFExport::setCustomRenderingWidgetsShown(bool visible)
{
	useCustomRenderingCheckBox->setShown(visible);
	customRenderingColorComboBox->setShown(visible);
	customRenderingFrequencySpinBox->setShown(visible);
	customRenderingAngleSpinBox->setShown(visible);
	customRenderingSpotFunctionComboBox->setShown(visible);
}

void Prefs_PDFExport::enableCustomRenderingWidgets(bool enabled)
{
	useCustomRenderingCheckBox->setEnabled(enabled);
	customRenderingColorComboBox->setEnabled(enabled);
	customRenderingFrequencySpinBox->setEnabled(enabled);
	customRenderingAngleSpinBox->setEnabled(enabled);
	customRenderingSpotFunctionComboBox->setEnabled(enabled);
}

void Prefs_PDFExport::setSolidsImagesWidgetsShown(bool visible)
{
	solidColorsLabel->setShown(visible);
	solidColorsLine->setShown(visible);
	useSolidColorProfileCheckBox->setShown(visible);
	solidColorProfileComboBox->setShown(visible);
	solidColorRenderingIntentComboBox->setShown(visible);
	imagesLabel->setShown(visible);
	imagesLine->setShown(visible);
	useImageProfileCheckBox->setShown(visible);
	doNotUseEmbeddedImageProfileCheckBox->setShown(visible);
	imageProfileComboBox->setShown(visible);
	imageRenderingIntentComboBox->setShown(visible);
}
void Prefs_PDFExport::enableSolidsImagesWidgets(bool enabled)
{
	//if we want to show/hide instead
	//setSolidsImagesWidgetsShown(enabled);
	useSolidColorProfileCheckBox->setEnabled(enabled);
	solidColorProfileComboBox->setEnabled(enabled);
	solidColorRenderingIntentComboBox->setEnabled(enabled);
	useImageProfileCheckBox->setEnabled(enabled);
	doNotUseEmbeddedImageProfileCheckBox->setEnabled(enabled);
	imageProfileComboBox->setEnabled(enabled);
	imageRenderingIntentComboBox->setEnabled(enabled);
}

void Prefs_PDFExport::enablePDFXWidgets(bool enabled)
{
	pdfx3OutputProfileComboBox->setEnabled(enabled);
	pdfx3InfoStringLineEdit->setEnabled(enabled);
}


void Prefs_PDFExport::enablePGI()
{
	bool setter=false;
	if (useImageProfileCheckBox->isChecked())
		setter = doNotUseEmbeddedImageProfileCheckBox->isChecked() ? true : false;
	imageProfileComboBox->setEnabled(setter);
	imageRenderingIntentComboBox->setEnabled(setter);
	doNotUseEmbeddedImageProfileCheckBox->setEnabled(useImageProfileCheckBox->isChecked());
}

void Prefs_PDFExport::enablePGI2()
{
	bool setter = doNotUseEmbeddedImageProfileCheckBox->isChecked() ? true : false;
	imageProfileComboBox->setEnabled(setter);
	imageRenderingIntentComboBox->setEnabled(setter);
}

void Prefs_PDFExport::enablePG()
{
	bool setter = useSolidColorProfileCheckBox->isChecked() ? true : false;

	solidColorProfileComboBox->setEnabled(setter);
	solidColorRenderingIntentComboBox->setEnabled(setter);
}

void Prefs_PDFExport::enablePDFX(int i)
{
	includeLayersCheckBox->setEnabled((i == 2) || (i == 5));
	if (useLayersRadioButton)
		useLayersRadioButton->setEnabled((i == 2) || (i == 5));
	if (m_doc != 0 && exportingPDF)
	{
		int currentEff = effectTypeComboBox->currentIndex();
		disconnect(effectTypeComboBox, SIGNAL(activated(int)), this, SLOT(SetEffOpts(int)));
		effectTypeComboBox->clear();
		effectTypeComboBox->addItem( tr("No Effect"));
		effectTypeComboBox->addItem( tr("Blinds"));
		effectTypeComboBox->addItem( tr("Box"));
		effectTypeComboBox->addItem( tr("Dissolve"));
		effectTypeComboBox->addItem( tr("Glitter"));
		effectTypeComboBox->addItem( tr("Split"));
		effectTypeComboBox->addItem( tr("Wipe"));
		if (i == 2)
		{
			effectTypeComboBox->addItem( tr("Push"));
			effectTypeComboBox->addItem( tr("Cover"));
			effectTypeComboBox->addItem( tr("Uncover"));
			effectTypeComboBox->addItem( tr("Fade"));
			effectTypeComboBox->setCurrentIndex(currentEff);
		}
		else
		{
			if (currentEff > 6)
			{
				currentEff = 0;
				effectTypeComboBox->setCurrentIndex(0);
				SetEffOpts(0);
				for (int pg = 0; pg<m_doc->Pages->count(); ++pg)
				{
					if (EffVal[pg].effectType > 6)
						EffVal[pg].effectType = 0;
				}
			}
			else
				effectTypeComboBox->setCurrentIndex(currentEff);
		}
		connect(effectTypeComboBox, SIGNAL(activated(int)), this, SLOT(SetEffOpts(int)));
	}

	if (i < 3)  // not PDF/X
	{
		enablePDFXWidgets(false);
		tabWidget->setTabEnabled(2, true);
		outputIntentionComboBox->setEnabled(true);
		useImageProfileCheckBox->setEnabled(true);
		emit hasInfo();
		if (m_doc != 0 && exportingPDF)
		{
			enabledEffectsCheckBox->setEnabled(true);
			embedAllButton->setEnabled(true);
			if (embeddedFontsListWidget->count() != 0)
				fromEmbedButton->setEnabled(true);
			toEmbedButton->setEnabled(true);
		}

		return;
	}
	// PDF/X is selected
	disconnect(outputIntentionComboBox, SIGNAL(activated(int)), this, SLOT(enableProfiles(int)));
	outputIntentionComboBox->setCurrentIndex(1);
	outputIntentionComboBox->setEnabled(false);
	enableProfiles(1);
	if ((i == 4) || (i == 5)) // X3 or X4, enforcing color profiles on images
	{
		useImageProfileCheckBox->setChecked(true);
		useImageProfileCheckBox->setEnabled(false);
	}
	if (m_doc != 0 && exportingPDF)
	{
//		EmbedFonts->setChecked(true);
		EmbedAll();
		enabledEffectsCheckBox->setChecked(false);
		enabledEffectsCheckBox->setEnabled(false);
//		EmbedFonts->setEnabled(false);
		fromEmbedButton->setEnabled(false);
		toEmbedButton->setEnabled(false);
		if (pdfx3InfoStringLineEdit->text().isEmpty())
			emit noInfo();
		else
			emit hasInfo();
	}
	enablePGI();
	pdfx3OutputProfileComboBox->setEnabled(true);
	pdfx3InfoStringLineEdit->setEnabled(true);
	tabWidget->setTabEnabled(2, false);
	connect(outputIntentionComboBox, SIGNAL(activated(int)), this, SLOT(enableProfiles(int)));
}

void Prefs_PDFExport::addPDFVersions(bool addPDFXStrings)
{
	disconnect(pdfVersionComboBox, SIGNAL(activated(int)), this, SLOT(enablePDFX(int)));
	int i = pdfVersionComboBox->currentIndex();
	pdfVersionComboBox->clear();
	pdfVersionComboBox->addItem("PDF 1.3 (Acrobat 4)");
	pdfVersionComboBox->addItem("PDF 1.4 (Acrobat 5)");
	pdfVersionComboBox->addItem("PDF 1.5 (Acrobat 6)");
	if (addPDFXStrings)
	{
		pdfVersionComboBox->addItem("PDF/X-1a");
		pdfVersionComboBox->addItem("PDF/X-3");
		pdfVersionComboBox->addItem("PDF/X-4");
	}
	else
		i=qMin(i,2);
	pdfVersionComboBox->setCurrentIndex(i);
	connect(pdfVersionComboBox, SIGNAL(activated(int)), this, SLOT(enablePDFX(int)));
}


void Prefs_PDFExport::enableEffects(bool enabled)
{
	effectsPageListWidget->setEnabled(enabled);
	showPagePreviewsCheckBox->setEnabled(enabled);
	displayDurationSpinBox->setEnabled(enabled);
	effectDurationSpinBox->setEnabled(enabled);
	effectTypeComboBox->setEnabled(enabled);
	if (enabled)
		SetEffOpts(effectTypeComboBox->currentIndex());
	else
	{
		effectMovingDirectionComboBox->setEnabled(false);
		effectInOutComboBox->setEnabled(false);
		effectDirectionComboBox->setEnabled(false);
		applyEffectToAllPagesPushButton->setEnabled(false);
	}
}
