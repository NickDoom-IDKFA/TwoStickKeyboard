//L/R Stick first?
//Up-Left-Down-Right moved?
//Up-Left-Down-Right other stick moved? (col. 5 for "N = None")
//R/L Released first (�᫨ �।. �� N), ����饭�᪠� ���� N (⮫쪮 ��� ��⮯������). � ��⮩ ������� ��� �� ����饭�᪠�, � ��� ࠧ �� ����.
//��� ���, � ��⮬ � � ०��� Fn. ��ࢮ� ������ ���� 㪠���� �ᥣ��. �᫨ ��⠫�� �� 㪠���� -- ��� ��⮬�⮬ (��� �㪢), ��� ⠪�� ��.

#define MAX_NAME 8

char KeyNames[2][4][5][3][3][MAX_NAME] = {
{
{ {{"4","$","F4"},{"5","%","F5"}},	{{"`","~"},{"1","!","F1"}},	{{"2","@","F2"},{"3","#","F3"}},	{{"6","^","F6"},{"7","&&","F7"}},	{{""},{""},{"Up"}} },
{ {{"A"},{"A"}},			{{"Tab"},{"Caps"}},		{{"A"},{"A"}},				{{"A"},{"A"}},				{{""},{""},{"Left"}} },
{ {{"A"},{"A"}},			{{"A"},{"A"}},			{{"A"},{"A"}},				{{"A"},{"A"}},				{{""},{""},{"Down"}} },
{ {{"A"},{"A"}},			{{"A"},{"A"}},			{{"A"},{"A"}},				{{"A"},{"A"}},				{{""},{""},{"Right"}} }
},{
{ {{"-","_","F11"},{"=","+","F12"}},	{{"8","*","F8"},{"9","(","F9"}},{{"0",")","F10"},{";",":"}},		{{"[","{"},{"]","}"}},			{{""},{""},{"Esc"}} },
{ {{"A"},{"A"}},			{{"A"},{"A"}},			{{"A"},{"A"}},				{{"A"},{"A"}},				{{""},{""},{"BkSp"}} },
{ {{"'","\""},{"\\","|"}},		{{"A"},{"A"}},			{{",","<"},{".",">"}},			{{"A"},{"/","?"}},			{{""},{""},{"Space"}} },
{ {{"PgUp"},{"Pause","","PrnSc"}},	{{"Home"},{"Ins"}},		{{"PgDn"},{"Fn"}},			{{"End"},{"Del"}},			{{""},{""},{"Enter"}} }
}
};
