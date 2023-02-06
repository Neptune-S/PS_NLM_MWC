function [dci_1_0_si_sib_1] = ps_nr_parse_dci_1_0_si_sib_1(dataBits)
  
dci_1_0_si_sib_1.fdra = sum(dataBits(1:11).*2.^[0:10]);
dci_1_0_si_sib_1.tdra = sum(dataBits(12:15).*2.^[0:3]);

if(dataBits(16) == 0)
  dci_1_0_si_sib_1.mapping = "Non-Interleaved";
else
  dci_1_0_si_sib_1.mapping = "Interleaved";
end

dci_1_0_si_sib_1.mcs = sum(dataBits(17:21).*2.^[0:4]);

dci_1_0_si_sib_1.rv  = sum(dataBits(21:22).*2.^[0:1]);

if(dataBits(23)==0)
  dci_1_0_si_sib_1.si_ind = "SIB_1";
else
  dci_1_0_si_sib_1.si_ind = "Other";
end
  
end  