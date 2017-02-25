#include "extendedbinomialtree.hpp"
#include <ql/quantlib.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/experimental/lattices/extendedbinomialtree.hpp>
#include <iostream>
#include <ctime>
#include <string>
#include <vector> 

using namespace QuantLib;

double time_elapse(std::clock_t c);
template<>
long long ExtendedBinomialTree_2<ExtendedJarrowRudd_2>::nb = 0;
template<>
long long ExtendedBinomialTree_2<ExtendedCoxRossRubinstein_2>::nb = 0;
template<>
long long ExtendedBinomialTree_2<ExtendedAdditiveEQPBinomialTree_2>::nb = 0;
template<>
long long ExtendedBinomialTree_2<ExtendedTrigeorgis_2>::nb = 0;
template<>
long long ExtendedBinomialTree_2<ExtendedTian_2>::nb = 0;
template<>
long long ExtendedBinomialTree_2<ExtendedLeisenReimer_2>::nb = 0;
template<>
long long ExtendedBinomialTree_2<ExtendedJoshi4_2>::nb = 0;

int main() {
 
    try {

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////Init////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

	

	std::clock_t    start;
	std::clock_t    step;
	start = std::clock();
	double time[2];


        // set up dates
        Calendar calendar = TARGET();
        Date todaysDate(15, May, 1998);
        Date settlementDate(17, May, 1998);
        Settings::instance().evaluationDate() = todaysDate;
	std::string method;

        // our options
        Option::Type type(Option::Put);
        Real underlying = 36;
        Real strike = 40;
        Spread dividendYield = 0.00;
        Rate riskFreeRate = 0.06;
        Volatility volatility = 0.20;
        Date maturity(17, May, 1999);
        DayCounter dayCounter = Actual365Fixed();
	Size timeSteps = 1000;
	Time t = 0;

	double europeanOptionNPV = 0.0;
	double bermudanOptionNPV = 0.0;
	double americanOptionNPV = 0.0;



///////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////Option Print//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << std::endl ;
	std::cout << "Option information:" << std::endl;
	std::cout << "Option type = "  << type << std::endl;
        std::cout << "Maturity = "        << maturity << std::endl;
        std::cout << "Underlying price = "        << underlying << std::endl;
        std::cout << "Strike = "                  << strike << std::endl;
        std::cout << "Risk-free interest rate = " << io::rate(riskFreeRate)
                  << std::endl;
        std::cout << "Dividend yield = " << io::rate(dividendYield)
                  << std::endl;
        std::cout << "Volatility = " << io::volatility(volatility)
                  << std::endl;
        std::cout << std::endl;

	// write column headings
        Size widths[] = { 35, 14, 14, 14, 14, 14, 11, 4};
        std::cout << std::setw(widths[0]) << std::left << "Method"
                  << std::setw(widths[1]) << std::right << "European"
                  << std::setw(widths[2]) << std::right << "Bermudan"
                  << std::setw(widths[3]) << std::right << "American"
		  << std::setw(widths[4]) << std::right << "Time"
		  << std::setw(widths[7]) << std::right	<< ""
		  << std::setw(widths[5]) << std::right << "Abs diff"
		  << std::setw(widths[7]) << std::right	<< ""
		  << std::setw(widths[6]) << std::right << "% diff"
                  << std::endl;

	std::vector<Date> exerciseDates;
        for (Integer i=1; i<=4; i++)
            exerciseDates.push_back(settlementDate + 3*i*Months);

        boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(maturity));

        boost::shared_ptr<Exercise> bermudanExercise(new BermudanExercise(exerciseDates));

        boost::shared_ptr<Exercise> americanExercise(new AmericanExercise(settlementDate, maturity));



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

	//payoff
	boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));
	//Underlying
        Handle<Quote> underlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlying)));
	//FlatDividendTS
	Handle<YieldTermStructure> flatDividendTS(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, dividendYield, dayCounter)));
	//flatTerStructure
	Handle<YieldTermStructure> flatTermStructure(boost::shared_ptr<YieldTermStructure>(new FlatForward(settlementDate, riskFreeRate, dayCounter)));
	//flatVolTS
	Handle<BlackVolTermStructure> flatVolTS(boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(settlementDate, calendar, volatility, dayCounter)));
	//process
	boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(new BlackScholesMertonProcess(underlyingH, flatDividendTS, flatTermStructure, flatVolTS));

	// options
        VanillaOption europeanOption(payoff, europeanExercise);
        VanillaOption bermudanOption(payoff, bermudanExercise);
        VanillaOption americanOption(payoff, americanExercise);

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////pricing////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


	// Binomial method: Jarrow-Rudd
        method = "Binomial Jarrow-Rudd";
	step = std::clock();
	europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<JarrowRudd>(bsmProcess,timeSteps)));
	bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<JarrowRudd>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<JarrowRudd>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[0] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[0] << " ms"
                  << std::endl;

        method = "Ext Binomial Jarrow-Rudd";
	step = std::clock();
	europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedJarrowRudd_2>(bsmProcess,timeSteps)));
	bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedJarrowRudd_2>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedJarrowRudd_2>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[1] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[1] << " ms"
		  << std::setw(widths[5]) << std::right << time[1]  - time[0] << " ms"
		  << std::setw(widths[6]) << std::right << static_cast<int>(((time[1]/time[0])-1) * 100 )<< " %"
                  << std::endl;
	
	// Binomial method: Cox-Ross-Rubinstein
	method = "Binomial Cox-Ross-Rubinstein";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeSteps)));
	bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<CoxRossRubinstein>(bsmProcess, timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[0] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[0] << " ms"
                  << std::endl;

	method = "Ext Binomial Cox-Ross-Rubinstein";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedCoxRossRubinstein_2>(bsmProcess, timeSteps)));
	bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedCoxRossRubinstein_2>(bsmProcess, timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedCoxRossRubinstein_2>(bsmProcess, timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[1] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[1] << " ms"
		  << std::setw(widths[5]) << std::right << time[1]  - time[0] << " ms"
		  << std::setw(widths[6]) << std::right << static_cast<int>(((time[1]/time[0])-1) * 100 )<< " %"
                  << std::endl;

 	// Binomial method: Additive equiprobabilities
        method = "Additive equiprobabilities";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<AdditiveEQPBinomialTree>(bsmProcess, timeSteps)));
	bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<AdditiveEQPBinomialTree>(bsmProcess, timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<AdditiveEQPBinomialTree>(bsmProcess, timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[0] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[0] << " ms"
                  << std::endl;

        method = "Ext Additive equiprobabilities";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedAdditiveEQPBinomialTree_2>(bsmProcess, timeSteps)));
	bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedAdditiveEQPBinomialTree_2>(bsmProcess, timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedAdditiveEQPBinomialTree_2>(bsmProcess, timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[1] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[1] << " ms"
		  << std::setw(widths[5]) << std::right << time[1]  - time[0] << " ms"
		  << std::setw(widths[6]) << std::right << static_cast<int>(((time[1]/time[0])-1) * 100 )<< " %"
                  << std::endl;

	// Binomial method: Binomial Trigeorgis
        method = "Binomial Trigeorgis";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Trigeorgis>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Trigeorgis>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Trigeorgis>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[0] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[0] << " ms"
                  << std::endl;

        method = "Ext Binomial Trigeorgis";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedTrigeorgis_2>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedTrigeorgis_2>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedTrigeorgis_2>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[1] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[1] << " ms"
		  << std::setw(widths[5]) << std::right << time[1]  - time[0] << " ms"
		  << std::setw(widths[6]) << std::right << static_cast<int>(((time[1]/time[0])-1) * 100 )<< " %"
                  << std::endl;

	// Binomial method: Binomial Tian
        method = "Binomial Tian";
	step = std::clock();
	europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Tian>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Tian>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Tian>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[0] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[0] << " ms"
                  << std::endl;

        method = "Ext Binomial Tian";
	step = std::clock();
	europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedTian_2>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedTian_2>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedTian_2>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[1] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[1] << " ms"
		  << std::setw(widths[5]) << std::right << time[1]  - time[0] << " ms"
		  << std::setw(widths[6]) << std::right << static_cast<int>(((time[1]/time[0])-1) * 100 )<< " %"
                  << std::endl;

	// Binomial method: Binomial Leisen-Reimer
        method = "Binomial Leisen-Reimer";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<LeisenReimer>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<LeisenReimer>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<LeisenReimer>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[0] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[0] << " ms"
                  << std::endl;

        method = "Ext Binomial Leisen-Reimer";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedLeisenReimer_2>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedLeisenReimer_2>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedLeisenReimer_2>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[1] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[1] << " ms"
		  << std::setw(widths[5]) << std::right << time[1]  - time[0] << " ms"
		  << std::setw(widths[6]) << std::right << static_cast<int>(((time[1]/time[0])-1) * 100 )<< " %"
                  << std::endl;

	// Binomial method: Binomial Joshi
        method = "Binomial Joshi";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Joshi4>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Joshi4>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<Joshi4>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[0] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[0] << " ms"
                  << std::endl;

        method = "Ext Binomial Joshi";
	step = std::clock();
        europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedJoshi4_2>(bsmProcess,timeSteps)));
        bermudanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedJoshi4_2>(bsmProcess,timeSteps)));
        americanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new BinomialVanillaEngine<ExtendedJoshi4_2>(bsmProcess,timeSteps)));

	europeanOptionNPV = europeanOption.NPV();
	bermudanOptionNPV = bermudanOption.NPV();
	americanOptionNPV = americanOption.NPV();
	
	time[1] = time_elapse(step);

        std::cout << std::setw(widths[0]) << std::left << method
                  << std::fixed
                  << std::setw(widths[1]) << std::right << europeanOptionNPV
                  << std::setw(widths[2]) << std::right << bermudanOptionNPV
                  << std::setw(widths[3]) << std::right << americanOptionNPV
		  << std::setw(widths[4]) << std::right << time[1] << " ms"
		  << std::setw(widths[5]) << std::right << time[1]  - time[0] << " ms"
		  << std::setw(widths[6]) << std::right << static_cast<int>(((time[1]/time[0])-1) * 100 )<< " %"
                  << std::endl;

	time[0] = time_elapse(start);
	std::cout << std::endl;
     	std::cout << "Time: " << time[0] << " ms" << std::endl;
	std::cout << std::endl;

	std::cout << std::setw(widths[0]) << std::left << "Effective go through function: (including driftStep, upStep, probUp and dxStep)"<< std::endl;

	std::cout << std::setw(widths[0]) << std::left << "ExtendedJarrowRudd_2 "<< std::fixed
                  << std::setw(widths[1]) << std::right << ExtendedBinomialTree_2<ExtendedJarrowRudd_2>::getNb() << std::endl;
	std::cout << std::setw(widths[0]) << std::left << "ExtendedCoxRossRubinstein_2 " << std::fixed
                  << std::setw(widths[1]) << std::right << ExtendedBinomialTree_2<ExtendedCoxRossRubinstein_2>::getNb() << std::endl;
	std::cout << std::setw(widths[0]) << std::left << "ExtendedAdditiveEQPBinomialTree_2 " << std::fixed
                  << std::setw(widths[1]) << std::right << ExtendedBinomialTree_2<ExtendedAdditiveEQPBinomialTree_2>::getNb() << std::endl;
	std::cout << std::setw(widths[0]) << std::left << "ExtendedTrigeorgis_2 " << std::fixed
                  << std::setw(widths[1]) << std::right << ExtendedBinomialTree_2<ExtendedTrigeorgis_2>::getNb() << std::endl;
	std::cout << std::setw(widths[0]) << std::left << "ExtendedTian_2 " << std::fixed
                  << std::setw(widths[1]) << std::right << ExtendedBinomialTree_2<ExtendedTian_2>::getNb() << std::endl;
	std::cout << std::setw(widths[0]) << std::left << "ExtendedLeisenReimer_2 " << std::fixed
                  << std::setw(widths[1]) << std::right << ExtendedBinomialTree_2<ExtendedLeisenReimer_2>::getNb() << std::endl;
	std::cout << std::setw(widths[0]) << std::left << "ExtendedJoshi4_2 " << std::fixed
                  << std::setw(widths[1]) << std::right << ExtendedBinomialTree_2<ExtendedJoshi4_2>::getNb() << std::endl;
	
        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}


double time_elapse(std::clock_t c){
	/*std::cout << static_cast<double>(std::clock()) - static_cast<double>(c) << std::endl;*/
	return (std::clock() - c) / (double)(CLOCKS_PER_SEC / 1000);
}

