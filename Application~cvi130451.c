#include <formatio.h> // file to arary
#include <ansi_c.h> // calloc
#include <cvirte.h>		
#include <advanlys.h> // max min
#include <utility.h> // delay launch executable
#include <userint.h>
#include "Application.h"

//==============================================================================
// Constants
#define SAMPLE_RATE		0
#define NPOINTS			1

//==============================================================================
// Global variables
int waveInfo[2]; //waveInfo[0] = sampleRate
//waveInfo[1] = number of elements
double sampleRate = 0.0;
int actualpoints;
int npoints = 0;
double* waveData = 0;
double* envelope = 0;
double* derivate = 0;
double* filterData = 0;
double* display = 0;

double maxVal = 0.0;
double minVal = 0.0;
int sample;


int haveRaw = 0; // for display
int haveFiltered = 0;

int intervals = 100;
size_t HistogramArray[100]; //vector de frecventa
double axisArray[100];




int envelope_plot = 0;
int derivate_plot = 0;


static int panelHandle;
static int freqHandle;


double *convertedSpectrum, *autoSpectrum;
double df, pp, fp;
char unit[100] = "v";
WindowConst winconst;


int main(int argc, char* argv[])
{
	if (InitCVIRTE(0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel(0, "Application.uir", PANEL)) < 0)
		return -1;
		if ((freqHandle = LoadPanel(0, "Application.uir", PANEL_2)) < 0)
		return -1;


	SetCtrlAttribute(panelHandle, PANEL_APLICA, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_ENVELOPE, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_DERIVATE, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_NEXT, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_PREV, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_SAVE, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_RING, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_ALPHA, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_MEDIERE, ATTR_DIMMED, 1);

	SetCtrlAttribute(panelHandle, PANEL_MIN, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_MAX, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_MIN_INDEX, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_MAX_INDEX, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_MEAN, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_DISPERSION, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_MEDIAN, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_ZEROS, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_SKEWNESS, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_KURTOSIS, ATTR_DIMMED, 1);

	SetCtrlAttribute(panelHandle, PANEL_START_DISP, ATTR_DIMMED, 1);
	SetCtrlAttribute(panelHandle, PANEL_END_DISP, ATTR_DIMMED, 1);
	SetCtrlAttribute(freqHandle, PANEL_2_NEXT, ATTR_DIMMED, 1);
	SetCtrlAttribute(freqHandle, PANEL_2_PREV, ATTR_DIMMED, 1);

	DisplayPanel(panelHandle);
	RunUserInterface();
	DiscardPanel(panelHandle);

	if (waveData != NULL) {
		free(waveData);
		waveData = NULL;
	}

	if (filterData != NULL) {
		free(filterData);
		filterData = NULL;
	}


	if (envelope != NULL) {
		free(envelope);
		envelope = NULL;
	}

	if (display != NULL) {
		free(display);
		display = NULL;
	}

	if (derivate != NULL) {
		free(derivate);
		derivate = NULL;
	}




	return 0;
}

int CVICALLBACK LOAD_CALLBACK(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int maxIndex, minIndex;
	double mean = 0.0;
	double median = 0.0;
	double skewness = 0;
	double dispersion = 0.0;
	double kurtosis = 0.0;
	int zeros = 0;

	//aux pt skewness si kurtosis
	double medie = 0.0;
	double numarator = 0, numitor = 0;

	switch (event)
	{
	case EVENT_COMMIT:

		//LaunchExecutable("python main.py");			
		//Delay(7);

		FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
		sampleRate = waveInfo[SAMPLE_RATE];
		npoints = waveInfo[NPOINTS];
		actualpoints = waveInfo[1] + 211;
		waveData = (double*)calloc(npoints + 211, sizeof(double)); // completam cu 211 zerouri pt ca am 5.9904 sec
		envelope = (double*)calloc(npoints + 211, sizeof(double));
		display = (double*)calloc(sampleRate, sizeof(double));
		derivate = (double*)calloc(npoints + 211, sizeof(double));
		//spectru 
		autoSpectrum = (double*)calloc((npoints + 211)/2, sizeof(double));
		convertedSpectrum = (double*)calloc((npoints + 211)/2, sizeof(double));
		
		

		FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
		FileToArray("Envelope.txt", envelope, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);

		//in display pun ce vreau sa afisez
		for (int i = 0; i < sampleRate; i++)
		{
			display[i] = waveData[i];
		}

		//derivata 
		DifferenceEx(waveData, npoints, 1.0, waveData + 1, 1, waveData + npoints - 1, 1, DIFF_SECOND_ORDER_CENTRAL, derivate);


		//calcul skewness cu formula de la final
		for (int i = 0; i < 256; i++)
		{
			medie += waveData[i];
		}

		medie /= 256;


		for (int i = 1; i <= 256; i++)
		{
			numarator += pow((waveData[i] - medie), 3);
		}
		numarator /= 256;

		for (int i = 1; i <= 256; i++)
		{
			numitor += pow((waveData[i] - medie), 2);
		}
		numitor /= 256;
		numitor = pow(numitor, 3. / 2);

		skewness = numarator / numitor;
		SetCtrlVal(panel, PANEL_SKEWNESS, skewness);

		//calcul kurtosis cu formula de la final
		numarator = 0;
		numitor = 0;

		for (int i = 0; i < 256; i++)
		{
			numarator += pow((waveData[i] - medie), 4);
		}
		numarator /= 256;

		for (int i = 0; i < 256; i++)
		{
			numitor += pow((waveData[i] - medie), 2);
		}
		numitor /= 256;
		numitor = pow(numitor, 2);
		kurtosis = numarator / numitor;
		SetCtrlVal(panel, PANEL_KURTOSIS, kurtosis);


		//treceri prin 0
		for (int i = 0; i < sampleRate - 1; i++)
		{
			if ((display[i] < 0 && display[i + 1] >= 0) || (display[i] > 0 && display[i + 1] <= 0))
				zeros++;
		}
		SetCtrlVal(PANEL, PANEL_ZEROS, zeros);



		//afisare pe graf raw data pt secunda initiala
		DeleteGraphPlot(PANEL, PANEL_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_GRAPH_RAW, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);




		//maxim si minim + index
		MaxMin1D(display, sampleRate, &maxVal, &maxIndex, &minVal, &minIndex);
		SetCtrlVal(PANEL, PANEL_MAX, maxVal);
		SetCtrlVal(PANEL, PANEL_MIN, minVal);
		SetCtrlVal(PANEL, PANEL_MAX_INDEX, maxIndex);
		SetCtrlVal(PANEL, PANEL_MIN_INDEX, minIndex);

		//mean si dispersion
		Variance(display, sampleRate, &mean, &dispersion);
		SetCtrlVal(panel, PANEL_DISPERSION, dispersion);
		SetCtrlVal(panel, PANEL_MEAN, mean);

		//median
		Median(display, sampleRate, &median);
		SetCtrlVal(panel, PANEL_MEDIAN, median);
		SetCtrlVal(panel, PANEL_END_DISP, 1);

		//afisez histograma pe secunda initiala	
		Histogram(display, sampleRate, minVal, maxVal, HistogramArray, axisArray, intervals);
		DeleteGraphPlot(PANEL, PANEL_HISTOGRAM, -1, VAL_IMMEDIATE_DRAW);
		PlotXY(PANEL, PANEL_HISTOGRAM, axisArray, HistogramArray, intervals, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, MakeColor(51, 102, 153));






		//activare controale	
		SetCtrlAttribute(panelHandle, PANEL_APLICA, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_ENVELOPE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_SAVE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_RING, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_ALPHA, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_MEDIERE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_MIN, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_MAX, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_MIN_INDEX, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_MAX_INDEX, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_MEAN, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_DISPERSION, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_MEDIAN, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_ZEROS, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_SKEWNESS, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_KURTOSIS, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_NEXT, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_PREV, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_START_DISP, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_END_DISP, ATTR_DIMMED, 0);


		break;
	}
	return 0;
}







int CVICALLBACK APLICA_CALLBACK(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	//secunda de start si de stoop
	int start = 0;
	int stop = 0;
	GetCtrlVal(panel, PANEL_START_DISP, &start);
	GetCtrlVal(panel, PANEL_END_DISP, &stop);

	//folosit la vectorul display pentru afisare
	int cnt = 0;

	//filter type
	int option;
	GetCtrlVal(PANEL, PANEL_RING, &option);

	switch (event)
	{
	case EVENT_COMMIT:
		if (option == 1) // alpha filter
		{
			double alpha;
			GetCtrlVal(PANEL, PANEL_ALPHA, &alpha);
			filterData = (double*)calloc(npoints + 211, sizeof(double));

			filterData[0] = waveData[0];

			for (int i = 1; i < npoints; i++)
			{
				filterData[i] = (1 - alpha) * filterData[i - 1] + alpha * waveData[i];
			}
		}
		else if (option == 0) // mediere
		{
			filterData = (double*)calloc(npoints + 211, sizeof(double));
			int n;
			double sum0 = 0;
			GetCtrlVal(panel, PANEL_MEDIERE, &n);
			for (int i = 0; i < n; i++)
				sum0 += waveData[i];
			for (int i = 0; i < n; i++)
				filterData[i] = sum0 / n;
			for (int i = n; i < waveInfo[1]; i++) {
				sum0 -= waveData[i - n];
				sum0 += waveData[i];
				filterData[i] = sum0 / n;
			}
		}

		//afisare pe graph semnal filtrat
		for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
		{
			display[cnt] = filterData[i];
			cnt++;
		}

		DeleteGraphPlot(panel, PANEL_GRAPH_FILTERED, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_GRAPH_FILTERED, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, MakeColor(51, 102, 153));
		cnt = 0;

		//resetez ca sa nu mai arate derivata sau anvelopa daca erau visibile
		derivate_plot = 0;
		envelope_plot = 0;

		//activez derivata doar cand am semnal filtrat
		SetCtrlAttribute(panelHandle, PANEL_DERIVATE, ATTR_DIMMED, 0);

		break;
	}
	return 0;
}

int CVICALLBACK onNEXT(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int maxIndex, minIndex;
	double mean = 0.0;
	double median = 0.0;
	double dispersion = 0.0;
	int zeros = 0;
	double kurtosis = 0.0;

	int start, stop;

	GetCtrlVal(panel, PANEL_START_DISP, &start);
	GetCtrlVal(panel, PANEL_END_DISP, &stop);

	int cnt = 0;


	switch (event)
	{
	case EVENT_COMMIT:
		if (start <= 4 && stop <= 6 && start >= 0 && stop >= 1) // pot merge la o alta secunda doar daca exista
		{
			start++;
			stop++;

			SetCtrlVal(PANEL, PANEL_START_DISP, start);
			SetCtrlVal(PANEL, PANEL_END_DISP, stop);
		}

		//display raw
		for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
		{
			display[cnt] = waveData[i];
			cnt++;
		}

		double medie = 0;
		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			medie += waveData[i];
		}

		medie /= 256;
		double skewness = 0;
		double numarator = 0, numitor = 0;

		for (int i = 1; i <= 256; i++)
		{
			numarator += pow((waveData[i] - medie), 3);
		}
		numarator /= 256;

		for (int i = 1; i <= 256; i++)
		{
			numitor += pow((waveData[i] - medie), 2);
		}
		numitor /= 256;
		numitor = pow(numitor, 3. / 2);

		skewness = numarator / numitor;
		SetCtrlVal(panel, PANEL_SKEWNESS, skewness);


		numarator = 0;
		numitor = 0;

		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			numarator += pow((waveData[i] - medie), 4);
		}
		numarator /= 256;

		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			numitor += pow((waveData[i] - medie), 2);
		}
		numitor /= 256;
		numitor = pow(numitor, 2);

		kurtosis = numarator / numitor;
		SetCtrlVal(panel, PANEL_KURTOSIS, kurtosis);




		//treceri prin 0
		for (int i = 0; i < sampleRate - 1; i++)
		{
			if ((display[i] < 0 && display[i + 1] >= 0) || (display[i] > 0 && display[i + 1] <= 0))
				zeros++;
		}

		SetCtrlVal(panel, PANEL_ZEROS, zeros);


		Histogram(display, sampleRate, minVal, maxVal, HistogramArray, axisArray, intervals);

		DeleteGraphPlot(PANEL, PANEL_HISTOGRAM, -1, VAL_IMMEDIATE_DRAW);
		PlotXY(PANEL, PANEL_HISTOGRAM, axisArray, HistogramArray, intervals, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, MakeColor(51, 102, 153));

		DeleteGraphPlot(panel, PANEL_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_GRAPH_RAW, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);

		if (filterData != 0) // daca am un semnal filtrat il misc si pe el
		{
			cnt = 0;
			for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
			{
				display[cnt] = filterData[i];
				cnt++;
			}

			DeleteGraphPlot(panel, PANEL_GRAPH_FILTERED, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_GRAPH_FILTERED, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, MakeColor(51, 102, 153));
		}



		cnt = 0;


		MaxMin1D(display, sampleRate, &maxVal, &maxIndex, &minVal, &minIndex);
		SetCtrlVal(panel, PANEL_MAX, maxVal);
		SetCtrlVal(panel, PANEL_MIN, minVal);
		SetCtrlVal(panel, PANEL_MAX_INDEX, maxIndex);
		SetCtrlVal(panel, PANEL_MIN_INDEX, minIndex);

		//mean si dispersion
		Variance(display, sampleRate, &mean, &dispersion);
		SetCtrlVal(panel, PANEL_DISPERSION, dispersion);
		SetCtrlVal(panel, PANEL_MEAN, mean);

		//median
		Median(display, sampleRate, &median);
		SetCtrlVal(panel, PANEL_MEDIAN, median);


		envelope_plot = 0;
		derivate_plot = 0;
		break;
	}



	return 0;
}

int CVICALLBACK onPREV(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int maxIndex, minIndex;
	double mean = 0.0;
	double median = 0.0;
	double dispersion = 0.0;
	int zeros = 0;
	double skewness, kurtosis;
	int start = 0;
	GetCtrlVal(panel, PANEL_START_DISP, &start);

	int stop = 0;
	int cnt = 0;
	GetCtrlVal(panel, PANEL_END_DISP, &stop);

	switch (event)
	{
	case EVENT_COMMIT:
		if (start >= 1 && stop <= 6 && start <= 5 && stop >= 2)
		{
			start--;
			stop--;

			SetCtrlVal(PANEL, PANEL_START_DISP, start);
			SetCtrlVal(PANEL, PANEL_END_DISP, stop);

		}

		for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
		{
			display[cnt] = waveData[i];
			cnt++;



		}
		double medie = 0;
		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			medie += waveData[i];
		}

		medie /= 256;
		double numarator = 0, numitor = 0;

		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			numarator += pow((waveData[i] - medie), 3);
		}
		numarator /= 256;

		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			numitor += pow((waveData[i] - medie), 2);
		}
		numitor /= 256;
		numitor = pow(numitor, 3. / 2);

		skewness = numarator / numitor;
		SetCtrlVal(panel, PANEL_SKEWNESS, skewness);



		numarator = 0;
		numitor = 0;

		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			numarator += pow((waveData[i] - medie), 4);
		}
		numarator /= 256;

		for (int i = sampleRate * start; i < sampleRate * start + 256; i++)
		{
			numitor += pow((waveData[i] - medie), 2);
		}
		numitor /= 256;
		numitor = pow(numitor, 2);

		kurtosis = numarator / numitor;
		SetCtrlVal(panel, PANEL_KURTOSIS, kurtosis);

		for (int i = 0; i < sampleRate - 1; i++)
		{
			if ((display[i] < 0 && display[i + 1] >= 0) || (display[i] > 0 && display[i + 1] <= 0))
				zeros++;
		}


		Histogram(display, sampleRate, minVal, maxVal, HistogramArray, axisArray, intervals);

		DeleteGraphPlot(PANEL, PANEL_HISTOGRAM, -1, VAL_IMMEDIATE_DRAW);
		PlotXY(PANEL, PANEL_HISTOGRAM, axisArray, HistogramArray, intervals, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, MakeColor(51, 102, 153));

		DeleteGraphPlot(panel, PANEL_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_GRAPH_RAW, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);

		SetCtrlVal(panel, PANEL_ZEROS, zeros);

		if (filterData != 0)
		{
			cnt = 0;



			for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
			{
				display[cnt] = filterData[i];
				cnt++;
			}

			DeleteGraphPlot(panel, PANEL_GRAPH_FILTERED, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_GRAPH_FILTERED, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, MakeColor(51, 102, 153));
		}

		MaxMin1D(display, sampleRate, &maxVal, &maxIndex, &minVal, &minIndex);
		SetCtrlVal(panel, PANEL_MAX, maxVal);
		SetCtrlVal(panel, PANEL_MIN, minVal);
		SetCtrlVal(panel, PANEL_MAX_INDEX, maxIndex);
		SetCtrlVal(panel, PANEL_MIN_INDEX, minIndex);

		//mean si dispersion
		Variance(display, sampleRate, &mean, &dispersion);
		SetCtrlVal(panel, PANEL_DISPERSION, dispersion);
		SetCtrlVal(panel, PANEL_MEAN, mean);

		//median
		Median(display, sampleRate, &median);
		SetCtrlVal(panel, PANEL_MEDIAN, median);


		envelope_plot = 0;
		derivate_plot = 0;
		break;
	}



	return 0;
}

int CVICALLBACK ENVELOPE_CALLBACK(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int start = 0;
	GetCtrlVal(panel, PANEL_START_DISP, &start);

	int stop = 0;
	int cnt = 0;
	GetCtrlVal(panel, PANEL_END_DISP, &stop);
	switch (event)
	{
	case EVENT_COMMIT:
		if (envelope_plot == 0)
		{
			cnt = 0;
			for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
			{
				display[cnt] = envelope[i];
				cnt++;
			}
			envelope_plot = PlotY(panel, PANEL_GRAPH_RAW, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);
		}
		else
		{

			DeleteGraphPlot(PANEL, PANEL_GRAPH_RAW, envelope_plot, VAL_IMMEDIATE_DRAW);
			envelope_plot = 0;
		}



		break;
	}
	return 0;
}

int CVICALLBACK DERIVATE_CALLBACK(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{

	int start = 0;
	GetCtrlVal(panel, PANEL_START_DISP, &start);

	int stop = 0;
	int cnt = 0;
	GetCtrlVal(panel, PANEL_END_DISP, &stop);
	switch (event)
	{
	case EVENT_COMMIT:
		if (derivate_plot == 0)
		{
			cnt = 0;
			for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
			{
				display[cnt] = derivate[i];
				cnt++;
			}
			derivate_plot = PlotY(panel, PANEL_GRAPH_FILTERED, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);
		}
		else
		{

			DeleteGraphPlot(PANEL, PANEL_GRAPH_FILTERED, derivate_plot, VAL_IMMEDIATE_DRAW);
			derivate_plot = 0;
		}



		break;
	}
	return 0;
}



int CVICALLBACK SAVE_CALLBACK(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int start = 0;
	GetCtrlVal(panel, PANEL_START_DISP, &start);

	int stop = 0;
	GetCtrlVal(panel, PANEL_END_DISP, &stop);

	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	int imgHandle;
	char fileName[256] = { 0 }, fileName1[256] = { 0 };
	switch (event)
	{
	case EVENT_COMMIT:
		//luam data si ora
		GetSystemDate(&month, &day, &year);
		GetSystemTime(&hour, &minute, &second);

		//setam numele cu care salvam imaginea
		sprintf(fileName, "./Saved/RAW_DATA_%4d.%2d.%2d_%2d-%2d-%2d_Window_%d_%d.jpg", year, month, day, hour, minute, second, start, stop);
		//salvam imaginea
		GetCtrlDisplayBitmap(panel, PANEL_GRAPH_RAW, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_PROGRESSIVE, 100);

		//setam numele cu care salvam imaginea
		sprintf(fileName1, "./Saved/FILTERED_DATA__%4d.%2d.%2d_%2d-%2d-%2d_Window_%d_%d.jpg", year, month, day, hour, minute, second, start, stop);
		//salvam imagine
		GetCtrlDisplayBitmap(panel, PANEL_GRAPH_FILTERED, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName1, JPEG_PROGRESSIVE, 100);

		DiscardBitmap(imgHandle);

		break;
	}
	return 0;
}



int CVICALLBACK onPanel(int panel, int event, void* callbackData,
	int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_CLOSE:
		QuitUserInterface(0);
		break;
	}
	return 0;
}

int CVICALLBACK SWITCH_CALLBACK (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//schimbarea panoului
			if(panel == panelHandle)
			{
				SetCtrlVal(freqHandle, PANEL_2_SWITCH_PANELS_1, 1);
				DisplayPanel(freqHandle);
				HidePanel(panel);
				

				
				
				
				
				
			}
			else
			{		
				SetCtrlVal(panelHandle,PANEL_SWITCH_PANELS, 0);
				DisplayPanel(panelHandle);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}



int CVICALLBACK onNEXT_2(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int nrForDownSample = sampleRate/4;
	double downSample[nrForDownSample], downSamplefilter[nrForDownSample];
	double autospecter[nrForDownSample/2], convertedspectre[nrForDownSample/2], autospecterfilter[nrForDownSample/2], convertedspectrefilter[nrForDownSample/2];;
	int nrForFerestruire = sampleRate;
	double ferestruire[nrForFerestruire];
	double *filter = malloc(sampleRate * sizeof(double));
	int filterType;
	int start, stop;
	int opt;

	GetCtrlVal(panel, PANEL_2_START_DISP, &start);
	GetCtrlVal(panel, PANEL_2_END_DISP, &stop);

	int cnt = 0;


	switch (event)
	{
	case EVENT_COMMIT:
		if (start <= 4 && stop <= 6 && start >= 0 && stop >= 1) // pot merge la o alta secunda doar daca exista
		{
			start++;
			stop++;

			SetCtrlVal(freqHandle, PANEL_2_START_DISP, start);
			SetCtrlVal(freqHandle, PANEL_2_END_DISP, stop);
		}

		//display raw
		for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
		{
			display[cnt] = waveData[i];
			cnt++;
			
			
			
		}
		
		GetCtrlVal(freqHandle, PANEL_2_RING_3, &filterType);
		if(filterType == 0)
		{
			
			Elp_HPF(display, sampleRate, sampleRate, 1000, 0.1, 40.0, 4, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		else
		{
			Elp_HPF(display, sampleRate, sampleRate, 1000, 0.1, 40.0, 6, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		
		GetCtrlVal(freqHandle, PANEL_2_RING_2, &opt);
		if(opt == 0)
		{
			WelchWin (filter, nrForFerestruire);

		}
		else if (opt ==1)
		{
			HanWin (filter, nrForFerestruire);
		}
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_FILTER_WINDOW, filter, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		
		
		
		
		Copy1D(display, nrForFerestruire, ferestruire);
		if(opt == 0)
		{
			WelchWin (ferestruire, nrForFerestruire);
		}
		else if (opt ==1)
		{
			HanWin (ferestruire, nrForFerestruire);
		}
		
		


		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW_WINDOW, ferestruire, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		

		for (int i = 0; i < nrForDownSample; i++) {
        // Calculate the average of the next 4 elements from the display vector
        double sum = 0.0;
		double sumFilter = 0.0;
        for (int j = i * 4; j < (i + 1) * 4; j++) {
            sum += display[j];
			sumFilter += filter[j];
        }
        downSample[i] = sum / 4.0;
		downSamplefilter[i] = sumFilter / 4.0;
    }
		
		
		
		if (stop<=6) // pot merge la o alta secunda doar daca exista
		{

				ScaledWindowEx(downSample, nrForDownSample, RECTANGLE, 0, &winconst);
				AutoPowerSpectrum(downSample, nrForDownSample, 1.0/waveInfo[0], autospecter, &df);
				PowerFrequencyEstimate(autospecter, nrForDownSample/2, -1.0, winconst, df, 7, &fp, &pp);
				memset(unit, 0, sizeof(unit));
				SpectrumUnitConversion(autospecter, nrForDownSample/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedspectre, &unit);
				DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
				PlotY(freqHandle, PANEL_2_GRAPH_SPECTRU, convertedspectre, nrForDownSample/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				
				ScaledWindowEx(downSamplefilter, nrForDownSample, RECTANGLE, 0, &winconst);
				AutoPowerSpectrum(downSamplefilter, nrForDownSample, 1.0/waveInfo[0], autospecterfilter, &df);
				PowerFrequencyEstimate(autospecterfilter, nrForDownSample/2, -1.0, winconst, df, 7, &fp, &pp);
				memset(unit, 0, sizeof(unit));
				SpectrumUnitConversion(autospecterfilter, nrForDownSample/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedspectrefilter, &unit);
				DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
				PlotY(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, convertedspectrefilter, nrForDownSample/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				
				
				
				
		}



		
		break;
	}



	return 0;
}


int CVICALLBACK onPREV_2(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{

	int nrForDownSample = sampleRate/4;
	double downSample[nrForDownSample], downSamplefilter[nrForDownSample];
	double autospecter[nrForDownSample/2], convertedspectre[nrForDownSample/2], autospecterfilter[nrForDownSample/2], convertedspectrefilter[nrForDownSample/2];
		int nrForFerestruire = sampleRate;
	double ferestruire[nrForFerestruire];
	int start, stop;
		double *filter = malloc(sampleRate * sizeof(double));
	int filterType;
	int opt;

	GetCtrlVal(panel, PANEL_2_START_DISP, &start);
	GetCtrlVal(panel, PANEL_2_END_DISP, &stop);

	int cnt = 0;


	switch (event)
	{
	case EVENT_COMMIT:
		if (start >= 1 && stop <= 6 && start <= 5 && stop >= 2)
		{
			start--;
			stop--;

			SetCtrlVal(freqHandle, PANEL_2_START_DISP, start);
			SetCtrlVal(freqHandle, PANEL_2_END_DISP, stop);

		}

//display raw
		for (int i = sampleRate * start; i < sampleRate * stop - 1; i++)
		{
			display[cnt] = waveData[i];
			cnt++;
			
			
		}
		
		GetCtrlVal(freqHandle, PANEL_2_RING_3, &filterType);
		if(filterType == 0)
		{
			Elp_HPF(display, sampleRate, sampleRate, 1000, 0.1, 40.0, 4, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		else
		{
			Elp_HPF(display, sampleRate, sampleRate, 1000, 0.1, 40.0, 6, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		
		GetCtrlVal(freqHandle, PANEL_2_RING_2, &opt);
		if(opt == 0)
		{
			WelchWin (filter, nrForFerestruire);

		}
		else if (opt ==1)
		{
			HanWin (filter, nrForFerestruire);
		}
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_FILTER_WINDOW, filter, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		


		Copy1D(display, nrForFerestruire, ferestruire);
		if(opt == 0)
		{
			WelchWin (ferestruire, nrForFerestruire);
		}
		else if (opt ==1)
		{
			HanWin (ferestruire, nrForFerestruire);
		}

		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW_WINDOW, ferestruire, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW, display, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		
		
		

		
		
		for (int i = 0; i < nrForDownSample; i++) {
        double sum = 0.0;
		double sumFilter = 0.0;
        for (int j = i * 4; j < (i + 1) * 4; j++) {
            sum += display[j];
			sumFilter += filter[j];
        }
        downSample[i] = sum / 4.0;
		downSamplefilter[i] = sumFilter / 4.0;
    }
		
		
		if (start>=0) // pot merge la o alta secunda doar daca exista
		{

				ScaledWindowEx(downSample, nrForDownSample, RECTANGLE, 0, &winconst);
				AutoPowerSpectrum(downSample, nrForDownSample, 1.0/waveInfo[0], autospecter, &df);
				PowerFrequencyEstimate(autospecter, nrForDownSample/2, -1.0, winconst, df, 7, &fp, &pp);
				memset(unit, 0, sizeof(unit));
				SpectrumUnitConversion(autospecter, nrForDownSample/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedspectre, &unit);
				DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
				PlotY(freqHandle, PANEL_2_GRAPH_SPECTRU, convertedspectre, nrForDownSample/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				
				ScaledWindowEx(downSamplefilter, nrForDownSample, RECTANGLE, 0, &winconst);
				AutoPowerSpectrum(downSamplefilter, nrForDownSample, 1.0/waveInfo[0], autospecterfilter, &df);
				PowerFrequencyEstimate(autospecterfilter, nrForDownSample/2, -1.0, winconst, df, 7, &fp, &pp);
				memset(unit, 0, sizeof(unit));
				SpectrumUnitConversion(autospecterfilter, nrForDownSample/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedspectrefilter, &unit);
				DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
				PlotY(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, convertedspectrefilter, nrForDownSample/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
		}



		
		break;
	}



	return 0;
}



int CVICALLBACK onRING (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	int nr = actualpoints/2;
	int opt;
	int filterType;
	double * filter = malloc(actualpoints * sizeof(double));
	double *smooth= malloc(actualpoints * sizeof(double));

	int displayType = 0;
	double *ferestruire = malloc(actualpoints * sizeof(double));
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(freqHandle, PANEL_2_RING, &displayType);
			switch(displayType)
			{	case 0:
					SetCtrlAttribute(freqHandle, PANEL_2_NEXT, ATTR_DIMMED, 1);
					SetCtrlAttribute(freqHandle, PANEL_2_PREV, ATTR_DIMMED, 1);

		
					


					ScaledWindowEx(waveData, actualpoints, RECTANGLE, 0, &winconst);
					AutoPowerSpectrum(waveData, actualpoints, 1.0/actualpoints, autoSpectrum, &df);
					PowerFrequencyEstimate(autoSpectrum, actualpoints/2, -1.0, winconst, df, 7, &fp, &pp);
					SpectrumUnitConversion(autoSpectrum, actualpoints/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedSpectrum, &unit);
					memset(unit, 0, sizeof(unit));
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
					PlotY(freqHandle, PANEL_2_GRAPH_SPECTRU, convertedSpectrum, actualpoints/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
					
					
					GetCtrlVal(freqHandle, PANEL_2_RING_2, &opt);
					Copy1D(waveData, actualpoints, ferestruire);
					if(opt == 0)
					{
						WelchWin (ferestruire, actualpoints);
					}
					else if (opt ==1)
					{
						HanWin (ferestruire, actualpoints);
					}

					DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, PANEL_2_GRAPH_RAW_WINDOW, ferestruire, actualpoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
					

					GetCtrlVal(freqHandle, PANEL_2_RING_3, &filterType);
					if(filterType == 0)
					{
						SavitzkyGolayFiltering (waveData, actualpoints, 1, 1, NULL, smooth);
						Elp_HPF(smooth, actualpoints, sampleRate, 1000, 0.1, 40.0, 4, filter);
						DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_FILTER, filter, actualpoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
					}
					else
					{
						SavitzkyGolayFiltering (waveData, actualpoints, 1, 1, NULL, smooth);
						Elp_HPF(smooth, actualpoints, sampleRate, 1000, 0.1, 40.0, 6, filter);
						DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_FILTER, filter, actualpoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
					}
							
					GetCtrlVal(freqHandle, PANEL_2_RING_2, &opt);
					if(opt == 0)
					{
						WelchWin (filter, actualpoints);

					}
					else if (opt ==1)
					{
						HanWin (filter, actualpoints);
					}
					
					DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, PANEL_2_GRAPH_FILTER_WINDOW, filter, actualpoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
									SetCtrlAttribute(freqHandle, PANEL_2_NEXT, ATTR_DIMMED, 1);
					SetCtrlAttribute(freqHandle, PANEL_2_PREV, ATTR_DIMMED, 1);
					DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, PANEL_2_GRAPH_RAW, waveData, actualpoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_BLUE);
		
					




					
			
			
			
					ScaledWindowEx(filter, actualpoints, RECTANGLE, 0, &winconst);
					AutoPowerSpectrum(filter, actualpoints, 1.0/actualpoints, autoSpectrum, &df);
					PowerFrequencyEstimate(autoSpectrum, actualpoints/2, -1.0, winconst, df, 7, &fp, &pp);
					SpectrumUnitConversion(autoSpectrum, actualpoints/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedSpectrum, &unit);
					memset(unit, 0, sizeof(unit));
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
					PlotY(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, convertedSpectrum, actualpoints/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
					
					
						

	
				
						
			
						
					
				break;
				case 1:
					
					SetCtrlAttribute(freqHandle, PANEL_2_NEXT, ATTR_DIMMED, 0);
					SetCtrlAttribute(freqHandle, PANEL_2_PREV, ATTR_DIMMED, 0);
					
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);

					
				break;
				case 2:
					SetCtrlAttribute(freqHandle, PANEL_2_NEXT, ATTR_DIMMED, 0);
					SetCtrlAttribute(freqHandle, PANEL_2_PREV, ATTR_DIMMED, 0);
					
					
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
					
					
				break;
			}

			break;
	}
	return 0;
}



int CVICALLBACK onNEXT_3 (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	int NSamples;
	double *display = NULL;

		
			double *ferestruire= NULL;
	double *filter = NULL;
	int filterType;
	int opt;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(freqHandle, PANEL_2_RING_4, &NSamples);
			display = realloc(display, NSamples * sizeof(double));
			GetCtrlVal(freqHandle, PANEL_2_START_DISP_2, &sample);
						sample++;
			SetCtrlVal(freqHandle, PANEL_2_START_DISP_2, sample);
			int cnt = 0;
			for(int i=sample * NSamples; i<(sample+1) * NSamples; i++)
			{
				display[cnt] = waveData[i];
				cnt++;
			}
			
			DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_RAW, display, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
			
			
			
			double autoSpectrumRaw[NSamples], convertedSpectrumRaw[NSamples], autoSpectrumFilter[NSamples], convertedSpectrumFilter[NSamples];

			
			
			
			
			ScaledWindowEx(display, NSamples, RECTANGLE, 0, &winconst);
			AutoPowerSpectrum(display, NSamples, 1.0/NSamples, autoSpectrumRaw, &df);
			PowerFrequencyEstimate(autoSpectrumRaw, NSamples/2, -1.0, winconst, df, 7, &fp, &pp);
			SpectrumUnitConversion(autoSpectrumRaw, NSamples/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedSpectrumRaw, &unit);
			memset(unit, 0, sizeof(unit));
			DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
			PlotY(freqHandle, PANEL_2_GRAPH_SPECTRU, convertedSpectrumRaw, NSamples/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
			
			
			
			
			double *ferestruire= malloc(NSamples * sizeof(double));
			double *filter = malloc(NSamples * sizeof(double));
			double *smooth = malloc(NSamples * sizeof(double));
			
			
			GetCtrlVal(freqHandle, PANEL_2_RING_3, &filterType);
		if(filterType == 0)
		{
			SavitzkyGolayFiltering (display, NSamples, 1, 1, NULL, smooth);
			Elp_HPF(smooth, NSamples, sampleRate, 1000, 0.1, 40.0, 4, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		else
		{	SavitzkyGolayFiltering (display, NSamples, 1, 1, NULL, smooth);
			Elp_HPF(smooth, NSamples, sampleRate, 1000, 0.1, 40.0, 6, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		
		GetCtrlVal(freqHandle, PANEL_2_RING_2, &opt);
		if(opt == 0)
		{
			WelchWin (filter, NSamples);

		}
		else if (opt ==1)
		{
			HanWin (filter, NSamples);
		}
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_FILTER_WINDOW, filter, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		
		
		
		
		Copy1D(display, NSamples, ferestruire);
		if(opt == 0)
		{
			WelchWin (ferestruire, NSamples);
		}
		else if (opt ==1)
		{
			HanWin (ferestruire, NSamples);
		}
		
		


		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW, display, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW_WINDOW, ferestruire, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);

		ScaledWindowEx(filter, NSamples, RECTANGLE, 0, &winconst);
		AutoPowerSpectrum(filter, NSamples, 1.0/NSamples, autoSpectrumFilter, &df);
		PowerFrequencyEstimate(autoSpectrumFilter, NSamples/2, -1.0, winconst, df, 7, &fp, &pp);
		SpectrumUnitConversion(autoSpectrumFilter, NSamples/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedSpectrumFilter, &unit);
		memset(unit, 0, sizeof(unit));
		DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
		PlotY(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, convertedSpectrumFilter, NSamples/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);	
		
			
			
			

			break;
	}
	return 0;
}

int CVICALLBACK onPREV_3 (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	int NSamples;
	double *display = NULL;

	
	
	
	double *ferestruire= NULL;
	double *filter = NULL;
	int filterType;
	int start, stop;
	int opt;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(freqHandle, PANEL_2_RING_4, &NSamples);
			display = realloc(display, NSamples * sizeof(double));
			GetCtrlVal(freqHandle, PANEL_2_START_DISP_2, &sample);
			
			if(sample >=1)
			{
			sample--;
			SetCtrlVal(freqHandle, PANEL_2_START_DISP_2, sample);
			}

			int cnt = 0;
			for(int i=sample * NSamples; i<(sample+1) * NSamples; i++)
			{
				display[cnt] = waveData[i];
				cnt++;
			}
			
			DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_RAW, display, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
			
			double autoSpectrumRaw[NSamples], convertedSpectrumRaw[NSamples], autoSpectrumFilter[NSamples], convertedSpectrumFilter[NSamples];
			
			ScaledWindowEx(display, NSamples, RECTANGLE, 0, &winconst);
			AutoPowerSpectrum(display, NSamples, 1.0/NSamples, autoSpectrumRaw, &df);
			PowerFrequencyEstimate(autoSpectrumRaw, NSamples/2, -1.0, winconst, df, 7, &fp, &pp);
			SpectrumUnitConversion(autoSpectrumRaw, NSamples/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedSpectrumRaw, &unit);
			memset(unit, 0, sizeof(unit));
			DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
			PlotY(freqHandle, PANEL_2_GRAPH_SPECTRU, convertedSpectrumRaw, NSamples/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
			
			double *ferestruire= malloc(NSamples * sizeof(double));
			double *filter = malloc(NSamples * sizeof(double));
			double *smooth = malloc(NSamples * sizeof(double));
			
			
				GetCtrlVal(freqHandle, PANEL_2_RING_3, &filterType);
		if(filterType == 0)
		{
			SavitzkyGolayFiltering (display, NSamples, 1, 1, NULL, smooth);
			Elp_HPF(smooth, NSamples, sampleRate, 1000, 0.1, 40.0, 4, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		else
		{
			SavitzkyGolayFiltering (display, NSamples, 1, 1, NULL, smooth);
			Elp_HPF(display, NSamples, sampleRate, 1000, 0.1, 40.0, 6, filter);
			DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			PlotY(panel, PANEL_2_GRAPH_FILTER, filter, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		}
		
		GetCtrlVal(freqHandle, PANEL_2_RING_2, &opt);
		if(opt == 0)
		{
			WelchWin (filter, NSamples);

		}
		else if (opt ==1)
		{
			HanWin (filter, NSamples);
		}
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_FILTER_WINDOW, filter, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		
		
		
		
		Copy1D(display, NSamples, ferestruire);
		if(opt == 0)
		{
			WelchWin (ferestruire, NSamples);
		}
		else if (opt ==1)
		{
			HanWin (ferestruire, NSamples);
		}
		
		


		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW, display, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
		
		DeleteGraphPlot(panel, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
		PlotY(panel, PANEL_2_GRAPH_RAW_WINDOW, ferestruire, NSamples, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);

		ScaledWindowEx(filter, NSamples, RECTANGLE, 0, &winconst);
		AutoPowerSpectrum(filter, NSamples, 1.0/NSamples, autoSpectrumFilter, &df);
		PowerFrequencyEstimate(autoSpectrumFilter, NSamples/2, -1.0, winconst, df, 7, &fp, &pp);
		SpectrumUnitConversion(autoSpectrumFilter, NSamples/2, SPECTRUM_POWER, 0, 0, df, winconst, convertedSpectrumFilter, &unit);
		memset(unit, 0, sizeof(unit));
		DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
		PlotY(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, convertedSpectrumFilter, NSamples/2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);	
		
			
			

			break;
	}
	return 0;
}

int CVICALLBACK Set0 (int panel, int control, int event,
					  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			sample = -1;
			SetCtrlVal(freqHandle, PANEL_2_START_DISP_2, sample);
								DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_RAW_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_SPECTRU, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_FILTER_WINDOW, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(freqHandle, PANEL_2_GRAPH_FILTER, -1, VAL_IMMEDIATE_DRAW);
			break;
	}
	return 0;
}

int CVICALLBACK SAVE_CALLBACK_2 (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{


	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;

	int imgHandle;
	char fileName[256] = { 0 }, fileName1[256] = { 0 };
	switch (event)
	{
	case EVENT_COMMIT:
		//luam data si ora
		GetSystemDate(&month, &day, &year);
		GetSystemTime(&hour, &minute, &second);

		//setam numele cu care salvam imaginea
		sprintf(fileName, "./Saved_ETAPA_2/GRAPH_RAW_%4d.%2d.%2d_%2d-%2d-%2d_Window.jpg", year, month, day, hour, minute, second);
		//salvam imaginea
		GetCtrlDisplayBitmap(freqHandle, PANEL_2_GRAPH_RAW, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_PROGRESSIVE, 100);
		
				sprintf(fileName, "./Saved_ETAPA_2/GRAPH_FILTER_%4d.%2d.%2d_%2d-%2d-%2d_Window.jpg", year, month, day, hour, minute, second);
		//salvam imaginea
		GetCtrlDisplayBitmap(freqHandle, PANEL_2_GRAPH_FILTER, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_PROGRESSIVE, 100);
		
		
		
				sprintf(fileName, "./Saved_ETAPA_2/GRAPH_SPECTRU_%4d.%2d.%2d_%2d-%2d-%2d_Window.jpg", year, month, day, hour, minute, second);
		//salvam imaginea
		GetCtrlDisplayBitmap(freqHandle, PANEL_2_GRAPH_SPECTRU, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_PROGRESSIVE, 100);
		
		
		
				sprintf(fileName, "./Saved_ETAPA_2/GRAPH_RAW_WINDOW_%4d.%2d.%2d_%2d-%2d-%2d_Window.jpg", year, month, day, hour, minute, second);
		//salvam imaginea
		GetCtrlDisplayBitmap(freqHandle, PANEL_2_GRAPH_RAW_WINDOW, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_PROGRESSIVE, 100);
		
		
				sprintf(fileName, "./Saved_ETAPA_2/GRAPH_FILTER_WINDOW_%4d.%2d.%2d_%2d-%2d-%2d_Window.jpg", year, month, day, hour, minute, second);
		//salvam imaginea
		GetCtrlDisplayBitmap(freqHandle, PANEL_2_GRAPH_FILTER_WINDOW, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_PROGRESSIVE, 100);
		
		
						sprintf(fileName, "./Saved_ETAPA_2/GRAPH_SPECTRUM_FILTER_%4d.%2d.%2d_%2d-%2d-%2d_Window.jpg", year, month, day, hour, minute, second);
		//salvam imaginea
		GetCtrlDisplayBitmap(freqHandle, PANEL_2_GRAPH_SPECTRUM_FILTER, 1, &imgHandle);
		SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_PROGRESSIVE, 100);


		DiscardBitmap(imgHandle);

		break;
	}
	return 0;
}
