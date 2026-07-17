&#x20;Projektbericht – Gruppe T028 (Memory Controller)



\## Teilnehmer-Anteile



\- \*\*Efe Yörük (go93buj):\*\* Rahmenprogramm, Memory Controller, System Testing, Hilfe beim Projektbericht



\- \*\*Arda Şahin (go38fax):\*\* ROM, Memory Protection Unit, System Testing, Hilfe beim Parsing



\- \*\*Firat Yöndem (go38hot):\*\* Parsing, Projektbericht, Präsentation, Hilfe beim System Testing



\## Ergebnisse der Literaturrecherche



Jeder Computer muss Daten speichern und wieder abrufen können, damit die CPU auf sie zugreifen kann. Die Kommunikation zwischen CPU und Speicher wird nicht direkt, sondern über den Memory Controller abgewickelt, der als Schnittstelle zwischen CPU und den verschiedenen Speichermodulen eines Systems fungiert.



\*\*Memory Controller:\*\*

Ein Memory Controller ist eine digitale Schaltung, die den Datenfluss zwischen CPU und Hauptspeicher koordiniert und kontrolliert. Er übersetzt Speicherzugriffsanfragen der CPU in Steuerbefehle und bildet logische Adressen auf physische Speicherpositionen ab. Je nach Hersteller sind Memory Controller entweder als eigenständige Chips oder direkt in die CPU integriert. Unsere Implementierung umfasst drei Funktionalitäten: Memory Mapping, Speicherschutz durch die MPU und Data-Width-Adaptation.



\*\*Von Neumann vs. Harvard Architektur:\*\*

Die Von-Neumann-Architektur legt Programmcode und Daten im selben Speicher ab und teilt einen gemeinsamen Bus. Der Prozessor kann daher nicht gleichzeitig auf Daten und Instruktionen zugreifen — dies führt zu einem Flaschenhals und erfordert mindestens zwei Taktzyklen pro Befehl. Die Harvard-Architektur trennt Daten und Code in separate Speicher mit getrennten Bussen, wodurch ein Befehl in einem einzigen Taktzyklus ausgeführt werden kann. Moderne CPUs nutzen oft eine modifizierte Harvard-Architektur mit getrennten Caches, aber gemeinsamem Hauptspeicher. Unsere Implementierung folgt der Von-Neumann-Architektur: ROM und Hauptspeicher teilen sich einen gemeinsamen Adressraum.



\*\*Memory Mapping:\*\*

Memory Mapping ermöglicht es, verschiedene Arten von Speicher und Geräten über ein einheitliches Adressierungsschema anzusprechen. Anstatt dass der Prozessor wissen muss, ob er mit ROM, RAM oder einem Peripheriegerät kommuniziert, gibt er eine Adresse aus, und der Controller entscheidet anhand des Adressbereichs, wohin die Anfrage geleitet wird. In unserer Implementierung werden Adressen von 0x0 bis rom\_size auf den ROM-Bereich abgebildet, alle übrigen Adressen werden an den Hauptspeicher weitergeleitet.



\*\*Read Only Memory (ROM):\*\*

ROM ist ein nichtflüchtiger Datenspeicher, auf den nur lesend zugegriffen werden kann. Er wird typischerweise genutzt, um Firmware-Instruktionen, Boot-Code oder andere unveränderliche Daten dauerhaft bereitzuhalten. In unserer Implementierung wird der Inhalt des ROMs über den Konstruktor des ReadOnlyMemory-Moduls festgelegt und kann nicht durch Schreibzugriffe verändert werden. Lesezugriffe liefern den gespeicherten Wert nach einer konfigurierbaren Latenz zurück.



\*\*Memory Protection Unit (MPU):\*\*

Eine MPU ist eine Hardwarekomponente, die unerlaubte Zugriffe auf Speicherbereiche verhindert, ohne dabei eine Adressübersetzung vorzunehmen — im Gegensatz zur vollständigen MMU. In unserer Implementierung wird der Hauptspeicher in Blöcke fester Größe unterteilt. Sobald ein Benutzer in einen Block schreibt, gehört dieser Block fortan ihm. Die Benutzer 0 und 255 bilden Sonderfälle: Sie haben stets Zugriff auf alle Blöcke, und ein Schreibzugriff von Benutzer 255 gibt einen Block wieder frei.



\## Kurzer Überblick über den Code



Die Implementierung besteht aus drei SystemC-Modulen und einem C17-Rahmenprogramm.



\*\*MEMORY\_CONTROLLER\*\* ist das Hauptmodul mit einem `behaviour()` SC\_THREAD. Bei jedem Zugriff prüft es zunächst ob `addr < rom\_size` — falls ja, wird das ROM-Submodul über das interne Signal `rom\_read` aktiviert und auf `rom\_ready` gewartet. Falls nein, prüft die MPU via `accessAndModify()` die Zugriffsberechtigung, bevor der Zugriff über den Helper `doMemRead()` an den Hauptspeicher weitergeleitet wird. Data-Width-Adaptation wird für alle Zugriffe durchgeführt: Bei `wide=F` wird beim Lesen nur das LSB zurückgegeben, beim Schreiben wird das bestehende Wort gelesen, das Byte ersetzt und das vollständige Wort zurückgeschrieben. Nach jedem Zugriff wird `ready=1` gesetzt, bei Fehler zusätzlich `error=1`.



\*\*ReadOnlyMemory\*\* ist ein SC\_THREAD-basiertes Submodul mit einem `std::vector<uint32\_t>` als Speicher und konfigurierbarer Latenz. Schreibzugriffe führen sofort zu einem Fehler.



\*\*MemoryProtectionUnit\*\* ist ein SC\_METHOD-basiertes Submodul mit einer `std::unordered\_map<uint32\_t, uint8\_t>` zur Verwaltung der Blockbesitzverhältnisse. `accessAndModify()` prüft und aktualisiert den Besitzer bei jedem Zugriff.



Das \*\*Rahmenprogramm\*\* in C17 liest CLI-Parameter mit `getopt\_long` ein, parst eine CSV-Eingabedatei zu `Request`-Structs und startet die SystemC-Simulation über `runSimulation()`. Ergebnisse werden auf der Kommandozeile ausgegeben, optional wird ein VCD-Tracefile erstellt.



\## Metrik



\- Erfolgreicher ROM-Lesezugriff: ROM-Latenz + 3 Taktzyklen

\- Fehlerhafter ROM-Zugriff (Schreiben): 3 Taktzyklen

\- Erfolgreicher Hauptspeicherzugriff: Hauptspeicher-Latenz + 7 Taktzyklen

\- Fehlerhafter Hauptspeicherzugriff: 3 Taktzyklen



\## Messumgebung



Das Projekt wurde mit C++14, C17 und SystemC 2.3.3 unter Windows WSL und Linux implementiert. Als Entwicklungsumgebung wurde VSCode verwendet. Valgrind und der Address Sanitizer dienten zur Erkennung von Segmentation Faults und Memory Leaks.



\## Quellen



\- \[1] A Survey of Memory Controller Architectures: Design Trends and Performance Trade-offs – https://ijrar.org/papers/IJRAR22D3204.pdf

\- \[2] Why ROM is Called Non Volatile Memory? – https://www.geeksforgeeks.org/computer-organization-architecture/why-rom-is-called-non-volatile-memory/

\- \[3] What's difference between MMU and MPU? – https://www.geeksforgeeks.org/operating-systems/whats-difference-between-mmu-and-mpu/

\- \[4] Difference between Von Neumann and Harvard Architecture – https://www.geeksforgeeks.org/computer-organization-architecture/difference-between-von-neumann-and-harvard-architecture/

\- \[5] Von Neumann and Harvard Architectures – Engineering LibreTexts – https://eng.libretexts.org/Bookshelves/Electrical\_Engineering/Electronics/Implementing\_a\_One\_Address\_CPU\_in\_Logisim\_(Kann)/01:\_Introduction/1.03:\_Von\_Neumann\_and\_Harvard\_Architectures

\- \[6] Difference between Memory Mapped IO and IO Mapped IO – https://www.geeksforgeeks.org/computer-organization-architecture/difference-between-memory-mapped-io-and-io-mapped-io-with-reference-to-8085-microprocessor/

\- \[7] What Is Memory Mapped I/O? – https://laptopjudge.com/what-is-memory-mapped-io/

\- \[8] A Survey on Memory-centric Computer Architectures – ACM – https://dl.acm.org/doi/fullHtml/10.1145/3544974

\- \[9] ARM Developer Documentation: Memory Protection Unit – https://developer.arm.com/documentation/107565/latest/Memory-protection/Memory-Protection-Unit

\- \[10] Comparative Study Between Harvard and Von Neumann Architectures – ResearchGate – https://www.researchgate.net/publication/387022161\_Comparative\_Study\_Between\_Harvard\_and\_Von\_Neumann\_Architectures\_in\_Modern\_Computing\_Applications

\- \[11] The Myth of the Harvard Architecture – IEEE Annals (Preprint) – http://metalup.org/harvardarchitecture/The%20Myth%20of%20the%20Harvard%20Architecture.pdf



