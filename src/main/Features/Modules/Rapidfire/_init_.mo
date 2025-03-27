//RapidFire是单发武器自动化，通过本模块，可以按住鼠标自动开火
//因为手枪

alias RapidFire_Enable "mout_rapidfire_1;soundtips_1;alias +smartattack TakeAttackToRapidFire;alias -smartattack TakeAttackCancelRapidFire"
alias RapidFire_Disable "mout_rapidfire_0;soundtips_0;alias +smartattack +attack;alias -smartattack -attack"