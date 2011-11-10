
#root -q -b -x macros/TriggerEfficiencies.C\(\"$1\",\"HT200\",\"Fancy\",\"HT\",\""H_{T} [GeV]"\"\)
#root -q -b -x macros/TriggerEfficiencies.C\(\"$1\",\"Ele8\",\"Fancy\",\"HT\",\""H_{T} [GeV]"\"\)
#root -q -b -x macros/TriggerEfficiencies.C\(\"$1\",\"CaloIso\",\"Fancy\",\"HT\",\""H_{T} [GeV]"\"\)
#root -q -b -x macros/TriggerEfficiencies.C\(\"$1\",\"Ele25TriCentralJet30\",\"Fancy\",\"HT\",\""H_{T} [GeV]"\"\)
root -q -b -x macros/TriggerEfficienciesData.C\(\"$1\",\"Ele90\",\"EleX\",\"HT\",\""H_{T} [GeV]"\"\)
root -q -b -x macros/TriggerEfficienciesData.C\(\"$1\",\"Ele90\",\"EleX\",\"ElectronPT\",\""Electron P_{T} [GeV]"\"\)

