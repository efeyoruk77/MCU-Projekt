Ergebnisse der Literaturrecherche:

Jeder Computer muss Daten speichern und wieder abrufen können, damit die CPU auf sie zugreifen kann. Diese Aufgabe übernimmt der Speicher. Die Kommunikation zwischen CPU und Speicher wird jedoch nicht direkt, sondern über den Memory Controller abgewickelt. 

Memory Controller: Der Memory Controller fungiert als Schnittstelle zwischen CPU und den verschiedenen Speichermodulen eines Systems[1]. Seine Aufgabe ist die Speicherzugriffsanfragen der CPU in Befehle zu übersetzen, ein Prozess, bei dem von der CPU erzeugte Speicheradressen auf physische Positionen in den Speichermodulen abgebildet werden. Unsere Implementierung erfüllt genau diese Rolle und umfasst drei Funktionalitäten - Memory Mapping (ROM, Main Memory), Speicherschutz durch Memory Protection Unit und Data-Width-Adaptation.

Memory Mapping: Ein Computer besitzt mehrere Memory Module, die alle von der CPU zugegriffen werden. Anstatt dass der Prozessor wissen muss, ob er mit ROM, RAM oder einem Peripheriegerät kommuniziert, gibt er einfach eine Adresse aus, und der Controller entscheidet anhand des Adressbereichs, wohin die Anfrage geleitet werden soll. In unserer Implementierung werden die Adressen 0x0 bis rom_size auf einen ReadOnlyMemory(ROM)-Bereich abgebildet, während alle übrigen Adressen an den Hauptspeicher weitergeleitet werden.

Read Only Memory (ROM): Der ROM-Bereich enthält read-only Daten, die nur gelesen und nicht geändert werden können. Er wird typischerweise genutzt, um Firmware-Instruktionen, Boot-Code oder andere unveränderliche Daten dauerhaft und nichtflüchtig bereitzuhalten[2]. In unserer Implementierung wird der Inhalt des ROMs über den Konstruktor des ReadOnlyMemory-Moduls festgelegt und kann nicht durch Schreibzugriffe verändert werden. Lesezugriffe hingegen liefern den gespeicherten Wert nach einer konfigurierbaren Latenz zurück.

Memory Protection Unit (MPU): Mehrere Benutzer sollen sich meistens gemeinsame Speicher teilen. Das birgt Sicherheitsrisiken: Nicht jeder Benutzer soll auf die Daten anderer Benutzer zugreifen können. Eine Memory Protection Unit schränkt genau das ein, indem sie überprüft, ob ein Benutzer für einen bestimmten Speicherbereich zugriffsberechtigt ist, ohne dabei, anders als eine vollständige MMU, eine Adressübersetzung vorzunehmen[3]. In unserer Implementierung wird der Hauptspeicher in Blöcke fester Größe unterteilt. Sobald ein Benutzer in einen Block schreibt, "gehört" dieser Block fortan ihm, und nur er darf anschließend darauf zugreifen. Die Benutzer 0 und 255 bilden dabei Sonderfälle: Sie haben stets Zugriff auf alle Blöcke, und ein Zugriff von Benutzer 255 gibt einen Block zusätzlich wieder frei. 

TODO: Harvard vs von Neumann

Quellen:
[1] A Survey of Memory Controller Architectures: Design Trends and Performance Trade-offs https://ijrar.org/papers/IJRAR22D3204.pdf
[2] Why ROM is Called Non Volatile Memory? https://www.geeksforgeeks.org/computer-organization-architecture/why-rom-is-called-non-volatile-memory/
[3] What's difference between MMU and MPU? https://www.geeksforgeeks.org/operating-systems/whats-difference-between-mmu-and-mpu/ 