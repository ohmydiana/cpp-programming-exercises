// Úkolem je vytvořit třídu CMailLog, která bude zpracovávat logy z poštovního subsystému.
//
// Při analýze e-mailového provozu je potřeba získat informace o tom, kteří uživatelé, kdy a komu poslali e-mail. K tomu je potřeba analyzovat logu poštovního serveru. Třída CMailLog toto bude umožňovat.
//
// Vstupem pro třídu je log poštovního serveru. Poštovní server má logy ve formě:
//
// month day year hour:minute:sec relay_name mailID: message
// tedy například:
// Mar 29 2025 14:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz
// Mar 29 2025 14:50:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable
// Mar 29 2025 14:58:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Meeting this afternoon
// Mar 29 2025 15:04:18.345 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz
// Význam polí je zřejmý: měsíc (anglická zkratka, 3 písmena, první velké), den měsíce, rok, hodina, minuta a sekunda (s přesností na milisekundy), jméno serveru (DNS jméno), identifikátor e-mailu (textový řetězec, písmena a čísla) a zpráva. Poštovní server loguje zprávu při každé zajímavé události (např. přijetí e-mailu od klienta, doručení e-mailu do schránky, ...). Je běžné, že jeden zaslaný e-mail vygeneruje několik řádek logu, řádky logu, které se týkají zpracování jednoho e-mailu mají stejný identifikátor.
//
// Cílem při zpracování logu je propojit odesilatele, subjekt a příjemce a zaznamenat si pro každý takový e-mail čas doručení. Proto nás budou zajímat zprávy začínající from= a k nim odpovídající subject= a to=. Zprávy, které nezačínají ani from=, subject= ani to= přeskakujte. Pokud v logu není k dispozici subject=, pak jej ve výpisech nezobrazujte.
//
// Z ukázky je vidět, že logování jde téměř chronologicky, ale občas je některá logovací hláška opožděná. Můžete se spolehnout, že opožděně jsou vypsané pouze hlášení typu to= (tedy pro každý e-mail je dodržená sekvence: nejprve from=, potom subject= a nakonec jedno či více to=). Dále je vidět, že mohou být promíchaná hlášení, která se týkají paralelně zpracovávaných e-mailů. Konečně, je vidět, že jeden e-mail může vygenerovat více hlášení typu to=, například pokud byl doručen více příjemcům. Toto budeme považovat za dva e-maily, navíc každý doručený v jiný čas.
//
// Vytvářená třída bude umět takový log zpracovávat, ukládat a generovat z něj reporty. Musí se začlenit do rozhraní popsaného níže. Část rozhraní je implementovaná v testovacím prostředí (třídy CTimeStamp a CMail), proto jsou tyto třídy v bloku podmíněného překladu. Tyto třídy tedy nemusíte celé implementovat, nicméně pro účely testování budete muset dodělat jejich alespoň zjednodušenou implementaci ("mocking"). Aby šel výsledný program zkompilovat, je NUTNÉ tyto dvě třídy a jejich implementaci ponechat uvnitř bloku podmíněného překladu. Plně pak musíte implementovat třídu CMailLog a tu naopak ponechat mimo blok podmíněného překladu.
//
// CTimeStamp
// Tato třída implementuje časové razítko. Má následující metody:
//
// konstruktor, který vyplní vzniklou instanci,
// přetížený operátor <<, který zobrazí časové razítko v ISO formátu (YYYY-MM-DD HH24:MI:SS.UUU). Pro vlastní testování tato funkce není nezbytná, hodí se ale pro ladění,
// Metodu pro porovnání compare. Pro volání a . compare (b) bude vráceno kladné číslo pro a > b, 0 pro a == b a záporné číslo pro a < b.

// CMail
// Tato třída reprezentuje jeden doručený e-mail. Má následující metody:
//
// konstruktor, který vyplní vzniklou instanci,
// metody pro čtení členských proměnných (getter) - from, subject, to a timeStamp,
// metody pro porovnání časových razítek v záznamech e-mailu, návratová hodnota odpovídá výsledku porovnání, který vrací CTimeStamp::compare,
// přetížený operátor pro výstup, opět pouze pro účely ladění.

// CMailLog
// Tato třída reprezentuje zpracovaný log poštovního serveru. Má následující metody:
//
// implicitní konstruktor, který vytvoří prázdnou instanci,
// destruktor, který uvolní alokované prostředky,
// metodu parseLog, která zpracuje předaný log. Log je přístupný v podobě vstupního streamu. Metoda zpracovává vstup a ukládá informace o doručených e-mailech. Pokud je nějaká řádka na vstupu neplatná, metoda ji přeskočí a pokračuje ve zpracování další řádky logu. Návratovou hodnotou metody je počet doručených e-mailů, které byly při zpracování logu nalezené (tedy ne počet řádek logu). Metodu lze volat opakovaně. Zavolání přidává nově zpracované e-maily k těm, které již zpracovala dříve. Identifiátory e-mailů v jednom zpracovaném logu se neopakují v žádném dalším logu,
// metoda listMail vrátí seznam nalezených e-mailů doručených v zadaném období (od-do, včetně). Vrácený seznam bude seřazený podle času doručení vzestupně, pokud by dvojice mailů měla stejný čas doručení, zachová se pořadí z logu,
// metoda activeUsers uvažuje všechny e-maily, které byly v zadaném období (od-do, včetně) doručené. Pro tyto e-maily vrátí množinu e-mailových adres uživatelů, kteří byli odesilateli či adresáty.


#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <compare>
#include <functional>
#include <optional>

class CTimeStamp
{
  public:
    CTimeStamp ( int year, int month, int day, int hour, int minute, double sec ) :
    m_year(year), m_month(month), m_day(day), m_hour(hour), m_min(minute), m_sec(sec) {}

    int compare ( const CTimeStamp & x ) const
    {
        if (m_year != x.m_year) return m_year - x.m_year;
        if (m_month != x.m_month) return m_month - x.m_month;
        if (m_day != x.m_day) return m_day - x.m_day;
        if (m_hour != x.m_hour) return m_hour - x.m_hour;
        if (m_min != x.m_min) return m_min - x.m_min;
        if (m_sec != x.m_sec) return (m_sec > x.m_sec) ? 1 : -1;
        return 0;
    }

    friend std::ostream & operator <<   ( std::ostream & os, const CTimeStamp & x )
    {
        os << std::setw(4) << std::setfill('0') << x.m_year << "-"
          << std::setw(2) << std::setfill('0') << x.m_month << "-"
          << std::setw(2) << std::setfill('0') << x.m_day << " "
          << std::setw(2) << std::setfill('0') << x.m_hour << ":"
          << std::setw(2) << std::setfill('0') << x.m_min << ":"
          << std::fixed << std::setprecision(3) << x.m_sec;
        return os;
    }
  private:
    int m_year, m_month, m_day, m_hour, m_min;
    double m_sec;
};

class CMail
{
  public:
    CMail ( const CTimeStamp & timeStamp, const std::string & from, const std::string & to, const std::optional<std::string> & subject ) :
    m_timeStamp(timeStamp), m_from(from), m_to(to), m_subject(subject) {}

    int compareByTime ( const CTimeStamp & x ) const
    {
        return m_timeStamp.compare(x);
    }
    int compareByTime ( const CMail & x ) const
    {
        return m_timeStamp.compare(x.m_timeStamp);
    }
    const std::string & from () const
    {
        return m_from;
    }

    const std::string & to () const
    {
        return m_to;
    }
    const std::optional<std::string> & subject () const
    {
        return m_subject;
    }

    const CTimeStamp & timeStamp () const
    {
        return m_timeStamp;
    }

    friend std::ostream & operator << ( std::ostream & os, const CMail & x )
    {
        os << x.m_timeStamp;

        os << " " << x.m_from << " -> " << x.m_to;

        if (x.m_subject) {
            os << ", subject: " << *x.m_subject;
        }

        return os;
    }

  private:
    CTimeStamp m_timeStamp;
    std::string m_from, m_to;
    std::optional<std::string> m_subject;
};

// your code will be compiled in a separate namespace
namespace MysteriousNamespace {
#endif /* __PROGTEST__ */
//----------------------------------------------------------------------------------------

struct TListMail
{
public:
    TListMail (const CMail & logsInTimeRange, int order) : m_logsInTimeRange(logsInTimeRange), m_order(order) {}

    const CMail & logsInTimeRange () const
    {
        return m_logsInTimeRange;
    }

    const int & order () const
    {
        return m_order;
    }

    bool operator < (const TListMail & other) const
    {
        int cmp = m_logsInTimeRange.compareByTime(other.logsInTimeRange());
        if (cmp != 0)
        {
            return cmp < 0;
        }
        return m_order < other.m_order;
    }

private:
    CMail m_logsInTimeRange;
    int m_order = 0;
};

class CMailLog
{
  public:
    int parseLog ( std::istream & in )
    {
        std::string line;
        std::string code, mail, type;
        while (std::getline(in, line))
        {
            CTimeStamp time = parseLine(line, code, mail, type);

            if (type == " from")
            {
                fromAndSubject[code].first = mail;
            }
            else if (type == " subject")
            {
                fromAndSubject[code].second = mail;
            }
            else if (type == " to")
            {
                CMail Mmail(time, fromAndSubject[code].first, mail, fromAndSubject[code].second);
                sortedLogs.insert(TListMail(Mmail, logCount));
                logCount++;
            }
        }

        return logCount;
    }

    std::list<CMail> listMail ( const CTimeStamp & from, const CTimeStamp & to ) const
    {
        std::list<CMail> result;

        auto it = sortedLogs.lower_bound(TListMail(CMail(from, "", "", std::nullopt), 0));
        while (it != sortedLogs.end() && it->logsInTimeRange().compareByTime(to) <= 0)
        {
            result.push_back(it->logsInTimeRange());
            it++;
        }

        return result;
    }

    std::set<std::string> activeUsers ( const CTimeStamp & from, const CTimeStamp & to ) const
    {
        std::set<std::string> result;

        auto it = sortedLogs.lower_bound(TListMail(CMail(from, "", "", std::nullopt), 0));
        while (it != sortedLogs.end() && it->logsInTimeRange().compareByTime(to) <= 0)
        {
            result.insert(it->logsInTimeRange().from());
            result.insert(it->logsInTimeRange().to());
            it++;
        }

        return result;
    }

  private:
    std::map<std::string, std::pair<std::string, std::optional<std::string>>> fromAndSubject;
    std::set<TListMail> sortedLogs;
    int logCount = 0;


    CTimeStamp parseLine (std::string line, std::string & code, std::string & mail, std::string & type)
    {
        std::istringstream iss (line);

        std::string month, server;
        int day, year, hour, minute;
        double second;

        std::map<std::string, int> months = {{"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4}, {"May", 5}, {"Jun", 6},
                                             {"Jul", 7}, {"Aug", 8}, {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}};

        iss >> month >> day >> year >> hour;
        iss.ignore(1, ':');
        iss >> minute;
        iss.ignore(1, ':');
        iss >> second;

        iss >> server >> code;
        std::getline(iss, type, '=');
        std::getline(iss, mail);

        int monthInt = months[month];

        CTimeStamp time (year, monthInt, day, hour, minute, second);
        return time;

    }
};
//----------------------------------------------------------------------------------------

#ifndef __PROGTEST__
} // namespace
std::string             printMail       ( const std::list<CMail> & all )
{
  std::ostringstream oss;
  for ( const auto & mail : all )
    oss << mail << "\n";
  return oss . str ();
}
int                     main ()
{
  MysteriousNamespace::CMailLog m;
  std::list<CMail> mailList;
  std::set<std::string> users;
  std::istringstream iss;

  iss . clear ();
  iss . str (
    "Mar 29 2025 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
    "Mar 29 2025 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
    "Mar 29 2025 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
    "Mar 29 2025 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
    "Mar 29 2025 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
    "Mar 29 2025 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
    "Mar 29 2025 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
    "Mar 29 2025 14:58:32.000 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
    "Mar 29 2025 15:02:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n" );
  assert ( m . parseLog ( iss ) == 8 );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );

    // std::cout << "vypis:\n" << printMail(mailList) << "\n";

  assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner
)###" );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 14, 58, 32 ) );
  assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
)###" );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 30, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 30, 23, 59, 59 ) );
  assert ( printMail ( mailList ) == "" );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
  assert ( users == std::set<std::string>( { "CEO@fit.cvut.cz", "CIO@fit.cvut.cz", "HR-department@fit.cvut.cz", "PR-department@fit.cvut.cz", "archive@fit.cvut.cz", "boss13@fit.cvut.cz", "dean@fit.cvut.cz", "office13@fit.cvut.cz", "person3@fit.cvut.cz", "user76@fit.cvut.cz", "vice-dean@fit.cvut.cz" } ) );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 13, 59, 59 ) );
  assert ( users == std::set<std::string>( { "person3@fit.cvut.cz", "user76@fit.cvut.cz" } ) );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
