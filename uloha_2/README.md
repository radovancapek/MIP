# Zadání

Napiště kód, jenž vytvoří nejméně 4 vlákna. Každé z vláken chce pracovat s displejem a mít jej po určitou dobu vyhrazen jen pro sebe. K synchronizaci na displeji vlákna používají semafor (z mbed.com).
Vlákna se aktivují dotykem na displej, kdykoliv se dotknete, aktivuje se další vlákno v pořadí.

# Provedení

Každé vlákno má svůj čítač, který načítá a zobrazuje hodnotu na display, pokud má vlákno přístup. Vlákna se přepínají dotykem na display.
