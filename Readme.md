\# Projektbericht – Gruppe T028 (Memory Controller)



\## Teilnehmer-Anteile



\- \*\*Efe Yörük (go93buj):\*\* Rahmenprogramm, Memory Controller, System Testing, Hilfe beim Projektbericht

\- \*\*Arda Şahin (go38fax):\*\* ROM, Memory Protection Unit, System Testing, Hilfe beim Parsing

\- \*\*Firat Yöndem (go38hot):\*\* Parsing, Projektbericht, Präsentation, Hilfe beim System Testing



\## Ergebnisse der Literaturrecherche



Die Kommunikation zwischen CPU und Speicher wird über den Memory Controller abgewickelt, der als Schnittstelle zwischen CPU und den verschiedenen Speichermodulen eines Systems fungiert.



\*\*Memory Controller:\*\*

Ein Memory Controller ist eine digitale Schaltung, die den Datenfluss zwischen CPU und Hauptspeicher koordiniert. Er übersetzt Speicherzugriffsanfragen in Steuerbefehle und bildet logische Adressen auf physische Speicherpositionen ab. Unsere Implementierung umfasst drei Funktionalitäten: Memory Mapping, Speicherschutz durch die MPU und Data-Width-Adaptation.



\*\*Von Neumann vs. Harvard Architektur:\*\*

Die Von-Neumann-Architektur legt Programmcode und Daten im selben Speicher ab und teilt einen gemeinsamen Bus. Der Prozessor kann nicht gleichzeitig auf Daten und Instruktionen zugreifen — dies führt zu einem Flaschenhals und erfordert mindestens zwei Taktzyklen pro Befehl. Die Harvard-Architektur trennt Daten und Code in separate Speicher mit getrennten Bussen, wodurch ein Befehl in einem Taktzyklus ausgeführt werden kann. Moderne CPUs nutzen oft eine modifizierte Harvard-Architektur mit getrennten Caches. Unsere Implementierung folgt Von Neumann: ROM und Hauptspeicher teilen sich einen gemeinsamen Adressraum.



\*\*Memory Mapping:\*\*

Memory Mapping ermöglicht es, verschiedene Speichertypen über ein einheitliches Adressierungsschema anzusprechen. Der Controller entscheidet anhand des Adressbereichs, wohin eine Anfrage geleitet wird. In unserer Implementierung werden Adressen von 0x0 bis rom\_size auf den ROM-Bereich abgebildet, alle übrigen an den Hauptspeicher.



\*\*Read Only Memory (ROM):\*\*

ROM ist ein nichtflüchtiger Datenspeicher, auf den nur lesend zugegriffen werden kann. Er speichert typischerweise Firmware, Boot-Code oder unveränderliche Daten. In unserer Implementierung wird der ROM-Inhalt über den Konstruktor festgelegt und kann nicht verändert werden. Lesezugriffe liefern den Wert nach konfigurierbarer Latenz zurück.



\*\*Memory Protection Unit (MPU):\*\*

Eine MPU verhindert unerlaubte Zugriffe auf Speicherbereiche ohne Adressübersetzung — im Gegensatz zur MMU. Der Hauptspeicher wird in Blöcke fester Größe unterteilt. Sobald ein Benutzer schreibt, gehört der Block ihm. Benutzer 0 und 255 haben stets Zugriff; Benutzer 255 gibt Blöcke beim Schreiben wieder frei.



\## Kurzer Überblick über den Code



Die Implementierung besteht aus drei SystemC-Modulen und einem C17-Rahmenprogramm.



\*\*MEMORY\_CONTROLLER\*\* ist das Hauptmodul mit einem `behaviour()` SC\_THREAD. Bei jedem Zugriff prüft es ob `addr < rom\_size` — falls ja, wird das ROM-Submodul über `rom\_read` aktiviert. Falls nein, prüft die MPU via `accessAndModify()` die Berechtigung, bevor der Zugriff über `doMemRead()` an den Hauptspeicher geht. Bei `wide=F` wird beim Lesen nur das LSB zurückgegeben, beim Schreiben wird das bestehende Wort gelesen, das Byte ersetzt und zurückgeschrieben. Nach jedem Zugriff wird `ready=1` gesetzt, bei Fehler zusätzlich `error=1`.



\*\*ReadOnlyMemory\*\* ist SC\_THREAD-basiert mit `std::vector<uint32\_t>` und konfigurierbarer Latenz. Schreibzugriffe führen sofort zu einem Fehler.



\*\*MemoryProtectionUnit\*\* ist SC\_METHOD-basiert mit `std::unordered\_map<uint32\_t, uint8\_t>` zur Verwaltung der Blockbesitzverhältnisse.



Das \*\*Rahmenprogramm\*\* in C17 liest CLI-Parameter mit `getopt\_long` ein, parst eine CSV-Datei zu `Request`-Structs und startet die Simulation via `runSimulation()`. Ergebnisse werden ausgegeben, optional wird ein VCD-Tracefile erstellt.



\## Metrik



\- Erfolgreicher ROM-Lesezugriff: ROM-Latenz + 3 Taktzyklen

\- Fehlerhafter ROM-Zugriff: 3 Taktzyklen

\- Erfolgreicher Hauptspeicherzugriff: Hauptspeicher-Latenz + 7 Taktzyklen

\- Fehlerhafter Hauptspeicherzugriff: 3 Taktzyklen



\## Messumgebung



C++14, C17, SystemC 2.3.3 unter Windows WSL und Linux. IDE: VSCode. Valgrind und Address Sanitizer zur Erkennung von Segmentation Faults und Memory Leaks.



\## Quellen



\- \[1] Survey of Memory Controller Architectures – ijrar.org/papers/IJRAR22D3204.pdf

\- \[2] Why ROM is Called Non Volatile Memory – geeksforgeeks.org

\- \[3] Difference between MMU and MPU – geeksforgeeks.org

\- \[4] Von Neumann vs Harvard Architecture – geeksforgeeks.org

\- \[5] Von Neumann and Harvard Architectures – eng.libretexts.org

\- \[6] Memory Mapped IO vs IO Mapped IO – geeksforgeeks.org

\- \[7] What Is Memory Mapped I/O – laptopjudge.com

\- \[8] Survey on Memory-centric Computer Architectures – dl.acm.org/doi/fullHtml/10.1145/3544974

\- \[9] ARM MPU Documentation – developer.arm.com

\- \[10] Harvard vs Von Neumann in Modern Computing – researchgate.net/publication/387022161

\- \[11] The Myth of the Harvard Architecture – metalup.org/harvardarchitecture



